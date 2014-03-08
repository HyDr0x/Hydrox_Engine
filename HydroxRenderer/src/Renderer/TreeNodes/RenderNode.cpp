#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include "Renderer/TreeNodes/RenderNodeTemplates//ArrayDrawCommand.h"
#include "Renderer/TreeNodes/RenderNodeTemplates/IndexedDrawCommand.h"
#include "Renderer/TreeNodes/RenderNodeTemplates/StaticGeometryRenderCommand.h"
#include "Renderer/TreeNodes/RenderNodeTemplates/SkinnedGeometryRenderCommand.h"

namespace he
{
	namespace renderer
	{
    RenderNode::RenderNode(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones) : 
      m_maxMaterials(maxMaterials), 
      m_maxGeometry(maxGeometry), 
      m_maxBones(maxBones),
      m_materialCount(0), 
      m_vboSize(0), 
      m_iboSize(0), 
      m_instanceNumber(0)
    {
      GLuint size = sizeof(Material::MaterialData) * m_maxMaterials;
      m_materialBuffer.createBuffer(size, GL_STATIC_DRAW);
    }

    RenderNode::~RenderNode()
    {
    }

    bool RenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void RenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    void RenderNode::initialize(sg::GeoNode *node, util::SingletonManager *singletonManager, util::ResourceHandle frustumCullingShaderHandle)
    {
      m_materialManager = singletonManager->getService<MaterialManager>();
      m_modelManager = singletonManager->getService<ModelManager>();

      Mesh *mesh = m_modelManager->getObject(node->getMeshHandle());
      m_primitiveType = mesh->getPrimitiveType();
      unsigned int vertexStride = mesh->getVertexStride();

      if(mesh->getIndexCount() == 0)
      {
        m_drawCommand = new ArrayDrawCommand(m_primitiveType, vertexStride);
      }
      else
      {
        m_drawCommand = new IndexedDrawCommand(m_primitiveType, vertexStride, GLINDEXTYPE);
      }

      if(dynamic_cast<sg::AnimatedGeoNode*>(node) != nullptr)
      {
        m_geometryCommand = new SkinnedGeometryRenderCommand(m_maxBones);
      }
      else
      {
        m_geometryCommand = new StaticGeometryRenderCommand();
      }

      m_frustumCulling.initialize(singletonManager->getService<ComputeShaderManager>(), frustumCullingShaderHandle, m_maxGeometry);
    }

    bool RenderNode::insertGeometry(sg::GeoNode *node)
    {
      Mesh *mesh = m_modelManager->getObject(node->getMeshHandle());

      if(m_instanceNumber >= m_maxGeometry || m_primitiveType != mesh->getPrimitiveType())
      {
        return false;
      }

      if(!m_materialHandles.count(node->getMaterialHandle().getID()))
      {
        if(m_materialCount >= m_maxMaterials)
        {
          return false;
        }

        m_materialHandles[node->getMaterialHandle().getID()] = m_materialCount;

        Material *material = m_materialManager->getObject(node->getMaterialHandle());
        m_materialBuffer.setData(&material->getMaterialData(), m_materialCount * sizeof(Material::MaterialData), sizeof(Material::MaterialData));

        m_materialCount++;
      }

      if(m_meshHandles[node->getMeshHandle()].size() != 0)
      {
        m_instanceNumberChanged = true;
      }
      else
      {
        m_geometryChanged = true;

        m_vboSize += mesh->getVBOSize();
        m_iboSize += mesh->getIndexCount() * sizeof(GLuint);
      }

      m_meshHandles[node->getMeshHandle()].push_back(node);
      m_instanceNumber++;

      return true;
    }

    bool RenderNode::removeGeometry(sg::GeoNode *node)
    {
      unsigned int preDeleteSize = m_meshHandles[node->getMeshHandle()].size();

      m_meshHandles[node->getMeshHandle()].remove(node);

      if(preDeleteSize == m_meshHandles[node->getMeshHandle()].size())//the node wasnt in the list --> wrong render node
      {
        return false;
      }

      m_instanceNumberChanged = true;

      if(m_meshHandles[node->getMeshHandle()].size() == 0)
      {
        m_geometryChanged = true;

        m_meshHandles.erase(node->getMeshHandle());

        Mesh *mesh = m_modelManager->getObject(node->getMeshHandle());

        m_vboSize -= mesh->getVBOSize();
        m_iboSize -= mesh->getIndexCount() * sizeof(GLuint);
      }

      m_instanceNumber--;

      return true;
    }

    bool RenderNode::isEmpty()
    {
      return m_meshHandles.size() == 0;
    }

    void RenderNode::frustumCulling()
    {
      //updateBuffer();

      //getTransformationMatrixBuffer().bindBuffer(0);
      //m_bboxesBuffer.bindBuffer(1);
      //m_commandBuffer.bindBuffer(3);
      //m_cullingCommandBuffer.bindBuffer(4);
      //m_meshInstanceIndexBuffer.bindBuffer(5);

      //std::vector<GLuint> culledAABB(m_maxGeometry);
      //m_frustumCulling.cullAABB(culledAABB, m_instanceCounter);

      //getTransformationMatrixBuffer().unbindBuffer(0);
      //m_bboxesBuffer.unbindBuffer(1);
      //m_commandBuffer.unbindBuffer(3);
      //m_cullingCommandBuffer.unbindBuffer(4);
      //m_meshInstanceIndexBuffer.unbindBuffer(5);
    }

    void RenderNode::rasterizeGeometry()
    {
      updateBuffer();

      m_geometryCommand->bindTransformationMatrixBuffer();

      m_materialIndexBuffer.bindBuffer(1);
      m_materialBuffer.bindBuffer(1);

      m_drawCommand->multiDraw();

      m_materialBuffer.unBindBuffer();
      m_materialIndexBuffer.unbindBuffer(1);

      m_geometryCommand->unbindTransformationMatrixBuffer();
    }

    void RenderNode::updateBuffer()
    {
      if(m_geometryChanged || m_instanceNumberChanged)
      {
        resizeBuffer();

        m_geometryChanged = false;
        m_instanceNumberChanged = false;
      }

      unsigned int instanceIndex = 0;
      for(std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++)
      {
        for(std::list<sg::GeoNode*>::const_iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, instanceIndex++)
        {
          m_geometryCommand->fillCaches(*geometryIterator, instanceIndex);
        }
      }

      m_geometryCommand->fillBuffer(instanceIndex);
    }

    void RenderNode::resizeBuffer()
    {
      if(m_geometryChanged)
      {
        m_drawCommand->createBuffer(m_meshHandles.size(), m_vboSize, m_iboSize);

        unsigned int commandIndex = 0;
        for(std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, commandIndex++)
        {
          m_drawCommand->fillCaches(commandIndex, meshIterator->second.size(), m_modelManager->getObject(meshIterator->first));
        }

        m_drawCommand->fillBuffer();
      }
      else if(m_instanceNumberChanged)
      {
        unsigned int commandIndex = 0;
        for(std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less>::const_iterator commandIterator = m_meshHandles.begin(); commandIterator != m_meshHandles.end(); commandIterator++, commandIndex++)
        {
          m_drawCommand->updateCommandBuffer(commandIndex, commandIterator->second.size());
        }
      }

      m_geometryCommand->resizeMatrixBuffer(m_instanceNumber);
      
      std::vector<GLuint> meshIndices(m_instanceNumber);
      std::vector<GLuint> materialIndexData(m_instanceNumber);

      unsigned int meshIndex = 0;
      unsigned int instanceIndex = 0;
      for(std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, meshIndex++)
      {
        for(std::list<sg::GeoNode*>::const_iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, instanceIndex++)
        {
          materialIndexData[instanceIndex] = m_materialHandles[(*geometryIterator)->getMaterialHandle().getID()];
          meshIndices[instanceIndex] = meshIndex;
        }
      }

      m_materialIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_instanceNumber, 0, GL_STATIC_DRAW, &materialIndexData[0]);
      m_meshInstanceIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_instanceNumber, 0, GL_STATIC_DRAW, &meshIndices[0]);

      m_materialBuffer.uploadData();
    }
	}
}
