#include "Renderer/RenderTree/RenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/RenderTree/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    RenderNode::RenderNode(unsigned int maxMaterials, unsigned int maxGeometry) : 
      m_maxMaterials(maxMaterials), 
      m_maxGeometry(maxGeometry), 
      m_materialCount(0), 
      m_vboSize(0), 
      m_indexSize(0), 
      m_vertexStride(0), 
      m_instanceCounter(0), 
      m_primitiveType(~0)
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

    TreeNode* RenderNode::createNewNode(InsertGeometryTraverser* traverser)
    {
      return traverser->createNewNode(this);
    }

    void RenderNode::initialize(util::SingletonManager *singletonManager, util::ResourceHandle frustumCullingShaderHandle)
    {
      m_materialManager = singletonManager->getService<MaterialManager>();
      m_modelManager = singletonManager->getService<ModelManager>();

      m_frustumCulling.initialize(singletonManager->getService<ComputeShaderManager>(), frustumCullingShaderHandle, m_maxGeometry);
    }

    bool RenderNode::insertGeometry(sg::GeoNode *node)
    {
      Mesh *mesh = m_modelManager->getObject(node->getMeshHandle());

      if(m_primitiveType == ~0)
      {
        m_primitiveType = mesh->getPrimitiveType();
      }

      if(m_instanceCounter >= m_maxGeometry || m_primitiveType != mesh->getPrimitiveType())
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

        m_vertexStride = mesh->getVertexStride();//should be constant

        m_vboSize += mesh->getVBOSize();
        m_indexSize += mesh->getIndexCount() * sizeof(GLuint);
      }

      m_meshHandles[node->getMeshHandle()].push_back(node);
      m_instanceCounter++;

      return true;
    }

    bool RenderNode::removeGeometry(sg::GeoNode *node)
    {
      Mesh *mesh = m_modelManager->getObject(node->getMeshHandle());
      m_meshHandles[node->getMeshHandle()].remove(node);

      m_instanceNumberChanged = true;

      if(m_meshHandles[node->getMeshHandle()].size() == 0)
      {
        m_geometryChanged = true;

        m_meshHandles.erase(node->getMeshHandle());

        m_vboSize -= mesh->getVBOSize();
        m_indexSize -= mesh->getIndexCount() * sizeof(GLuint);
      }

      m_instanceCounter--;

      return m_meshHandles.size() == 0;
    }

    void RenderNode::updateBuffer()
    {
      if(m_geometryChanged || m_instanceNumberChanged)
      {
        resizeBuffer();

        m_geometryChanged = false;
        m_instanceNumberChanged = false;
      }

      unsigned int geometryCounter = 0;
      for(std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++)
      {
        for(std::list<sg::GeoNode*>::const_iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, geometryCounter++)
        {
          if((*geometryIterator)->isAnimatedGeoNode())
          {
            fillMatrixBuffer(static_cast<sg::AnimatedGeoNode*>(*geometryIterator), geometryCounter);
          }
          else
          {
            fillMatrixBuffer(static_cast<sg::GeoNode*>(*geometryIterator), geometryCounter);
          }
        }
      }

      uploadMatrices();
    }

    void RenderNode::frustumCulling()
    {
      updateBuffer();

      getTransformationMatrixBuffer().bindBuffer(0);
      m_bboxesBuffer.bindBuffer(1);
      m_commandBuffer.bindBuffer(3);
      m_cullingCommandBuffer.bindBuffer(4);
      m_meshInstanceIndexBuffer.bindBuffer(5);

      std::vector<GLuint> culledAABB(m_maxGeometry);
      m_frustumCulling.cullAABB(culledAABB, m_instanceCounter);

      getTransformationMatrixBuffer().unbindBuffer(0);
      m_bboxesBuffer.unbindBuffer(1);
      m_commandBuffer.unbindBuffer(3);
      m_cullingCommandBuffer.unbindBuffer(4);
      m_meshInstanceIndexBuffer.unbindBuffer(5);
    }

    void RenderNode::rasterizeGeometry()
    {
      updateBuffer();

      m_matrixBuffer.bindBuffer(0);
      m_materialIndexBuffer.bindBuffer(1);

      m_materialBuffer.bindBuffer(1);

      m_commandBuffer.bindBuffer();
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);
      m_meshIndexBuffer.bindBuffer();

      glMultiDrawElementsIndirect(m_primitiveType, GLINDEXTYPE, nullptr, m_meshHandles.size(), 0);

      m_meshIndexBuffer.unbindBuffer();
      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer();

      m_materialBuffer.unBindBuffer();

      m_materialIndexBuffer.unbindBuffer(1);
      m_matrixBuffer.unbindBuffer(0);
    }

    void RenderNode::resizeBuffer()
    {
      if(m_geometryChanged)
      {
        m_meshVertexBuffer.createBuffer(GL_ARRAY_BUFFER, m_vboSize, 0, GL_STATIC_DRAW, nullptr);
        m_meshIndexBuffer.createBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexSize, 0, GL_STATIC_DRAW, nullptr);
        m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * m_meshHandles.size(), 0, GL_STATIC_DRAW, nullptr);
        m_cullingCommandBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_meshHandles.size(), 0, GL_STATIC_DRAW, nullptr);
        m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_meshHandles.size() * 2, 0, GL_STATIC_DRAW, nullptr);

        std::vector<util::Vector<unsigned int, 4>> cullingCommandCache(m_meshHandles.size());
        std::vector<util::Vector<float, 4>> boundingBoxes(m_meshHandles.size() * 2);

        Mesh *mesh;

        unsigned int commandCounter = 0;
        unsigned int vertexOffset = 0;
        unsigned int indexOffset = 0;
        unsigned int vertexCount = 0;
        unsigned int indexCount = 0;
        unsigned int instanceCounter = 0;
        for(std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, commandCounter++)
        {
          mesh = m_modelManager->getObject(meshIterator->first);

          m_meshVertexBuffer.setData(vertexOffset, mesh->getVBOSize(), &(mesh->getVBOBuffer()[0]));
          vertexOffset += mesh->getVBOSize();

          m_meshIndexBuffer.setData(indexOffset, mesh->getIndexCount() * sizeof(GLuint), &(mesh->getIndexBuffer()[0]));
          indexOffset += mesh->getIndexCount() * sizeof(GLuint);

          DrawElementsIndirectCommand command;
          command.count = mesh->getIndexCount();
          command.instanceCount = meshIterator->second.size();
          command.firstIndex = indexCount;
          command.baseVertex = vertexCount;
          command.baseInstance = instanceCounter;

          m_commandBuffer.setData(commandCounter * sizeof(DrawElementsIndirectCommand), sizeof(DrawElementsIndirectCommand), &command);

          cullingCommandCache[commandCounter][0] = command.count;
          cullingCommandCache[commandCounter][1] = command.firstIndex;
          cullingCommandCache[commandCounter][2] = command.baseVertex;
          cullingCommandCache[commandCounter][3] = command.baseInstance;

          indexCount += mesh->getIndexCount();
          vertexCount += mesh->getVertexCount();
          instanceCounter += meshIterator->second.size();

          //update bbox data
          memcpy(&boundingBoxes[2 * commandCounter + 0][0], &mesh->getBBMin()[0], sizeof(util::Vector<float, 3>));
          memcpy(&boundingBoxes[2 * commandCounter + 1][0], &mesh->getBBMax()[0], sizeof(util::Vector<float, 3>));
        }

        m_bboxesBuffer.setData(0, sizeof(util::Vector<float, 4>) * m_meshHandles.size() * 2, &boundingBoxes[0]);
        m_cullingCommandBuffer.setData(0, sizeof(util::Vector<float, 4>) * m_meshHandles.size(), &cullingCommandCache[0]);
      }
      else if(m_instanceNumberChanged)
      {
        unsigned int commandCounter = 0;
        for(std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less>::const_iterator commandIterator = m_meshHandles.begin(); commandIterator != m_meshHandles.end(); commandIterator++, commandCounter++)
        {
          size_t size = commandIterator->second.size();
          m_commandBuffer.setData(commandCounter * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &size);
        }
      }

      resizeMatrixBuffer();
      
      std::vector<GLuint> meshIndex(m_instanceCounter);
      std::vector<GLuint> materialIndexData(m_instanceCounter);

      unsigned int meshCounter = 0;
      unsigned int geometryCounter = 0;
      for(std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, meshCounter++)
      {
        for(std::list<sg::GeoNode*>::const_iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, geometryCounter++)
        {
          materialIndexData[geometryCounter] = m_materialHandles[(*geometryIterator)->getMaterialHandle().getID()];
          meshIndex[geometryCounter] = meshCounter;
        }
      }

      m_materialIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_instanceCounter, 0, GL_STATIC_DRAW, &materialIndexData[0]);
      m_meshInstanceIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_instanceCounter, 0, GL_STATIC_DRAW, &meshIndex[0]);

      m_materialBuffer.uploadData();
    }
	}
}
