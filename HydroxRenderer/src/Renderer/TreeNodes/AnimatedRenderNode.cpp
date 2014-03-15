#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include "Renderer/TreeNodes/RenderNodeTemplates/ArrayDrawCommand.h"
#include "Renderer/TreeNodes/RenderNodeTemplates/IndexedDrawCommand.h"
#include "Renderer/TreeNodes/RenderNodeTemplates/StaticMatrixBuffer.h"
#include "Renderer/TreeNodes/RenderNodeTemplates/SkinnedMatrixBuffer.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

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
      m_instanceNumber(0),
      m_drawCommand(nullptr),
      m_matrixBuffer(nullptr)
    {
      GLuint size = sizeof(Material::MaterialData) * m_maxMaterials;
      m_materialBuffer.createBuffer(size, GL_STATIC_DRAW);
    }

    RenderNode::~RenderNode()
    {
      if(m_drawCommand != nullptr)
      {
        delete m_drawCommand;
      }

      if(m_matrixBuffer != nullptr)
      {
        delete m_matrixBuffer;
      }
    }

    bool RenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void RenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    void RenderNode::initialize(bool skinned, bool indexed, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager)
    {
      m_materialManager = singletonManager->getService<MaterialManager>();
      m_modelManager = singletonManager->getService<ModelManager>();

      m_primitiveType = primitiveType;

      if(indexed)
      {
        m_drawCommand = new IndexedDrawCommand(m_primitiveType, vertexStride, GLINDEXTYPE);
      }
      else
      {
        m_drawCommand = new ArrayDrawCommand(m_primitiveType, vertexStride);
      }

      if(skinned)
      {
        m_matrixBuffer = new SkinnedMatrixBuffer(m_maxBones);
      }
      else
      {
        m_matrixBuffer = new StaticMatrixBuffer();
      }

      m_frustumCulling.initialize(m_maxGeometry);
    }

    bool RenderNode::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());

      if(m_instanceNumber >= m_maxGeometry || m_primitiveType != mesh->getPrimitiveType())
      {
        return false;
      }

      if(!m_materialHandles.count(geometryContainer.getMaterialHandle()))
      {
        if(m_materialCount >= m_maxMaterials)
        {
          return false;
        }

        m_materialHandles[geometryContainer.getMaterialHandle()] = m_materialCount;

        Material *material = m_materialManager->getObject(geometryContainer.getMaterialHandle());
        m_materialBuffer.setData(&material->getMaterialData(), m_materialCount * sizeof(Material::MaterialData), sizeof(Material::MaterialData));

        m_materialCount++;
      }

      if(m_meshHandles[geometryContainer.getMeshHandle()].size() != 0)
      {
        m_instanceNumberChanged = true;
      }
      else
      {
        m_geometryChanged = true;

        m_vboSize += mesh->getVBOSize();
        m_iboSize += mesh->getIndexCount() * sizeof(GLuint);
      }

      m_meshHandles[geometryContainer.getMeshHandle()].push_back(geometryContainer);
      m_instanceNumber++;

      return true;
    }

    bool RenderNode::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      unsigned int preDeleteSize = m_meshHandles[geometryContainer.getMeshHandle()].size();

      std::list<xBar::StaticGeometryContainer>& geometryList = m_meshHandles[geometryContainer.getMeshHandle()];
      for(std::list<xBar::StaticGeometryContainer>::const_iterator geometryIterator = geometryList.begin(); geometryIterator != geometryList.end(); geometryIterator++)
      {
        if(*geometryIterator == geometryContainer)
        {
          geometryList.erase(geometryIterator);
          break;
        }
      }

      if(preDeleteSize == m_meshHandles[geometryContainer.getMeshHandle()].size())//the node wasnt in the list --> wrong render node
      {
        return false;
      }

      m_instanceNumberChanged = true;

      if(m_meshHandles[geometryContainer.getMeshHandle()].size() == 0)
      {
        m_geometryChanged = true;

        m_meshHandles.erase(geometryContainer.getMeshHandle());

        Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());

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

      m_matrixBuffer->bindTransformationMatrixBuffer();

      m_materialIndexBuffer.bindBuffer(1);
      m_materialBuffer.bindBuffer(1);

      m_drawCommand->multiDraw();

      m_materialBuffer.unBindBuffer();
      m_materialIndexBuffer.unbindBuffer(1);

      m_matrixBuffer->unbindTransformationMatrixBuffer();
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
      for(std::map<util::ResourceHandle, std::list<xBar::StaticGeometryContainer>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++)
      {
        for(std::list<xBar::StaticGeometryContainer>::const_iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, instanceIndex++)
        {
          m_matrixBuffer->fillCaches(*geometryIterator, instanceIndex);
        }
      }

      m_matrixBuffer->fillBuffer(instanceIndex);
    }

    void RenderNode::resizeBuffer()
    {
      if(m_geometryChanged)
      {
        m_drawCommand->createBuffer(m_meshHandles.size(), m_vboSize, m_iboSize);

        unsigned int commandIndex = 0;
        for(std::map<util::ResourceHandle, std::list<xBar::StaticGeometryContainer>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, commandIndex++)
        {
          m_drawCommand->fillCaches(commandIndex, meshIterator->second.size(), m_modelManager->getObject(meshIterator->first));
        }

        m_drawCommand->fillBuffer();
      }
      else if(m_instanceNumberChanged)
      {
        unsigned int commandIndex = 0;
        for(std::map<util::ResourceHandle, std::list<xBar::StaticGeometryContainer>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, commandIndex++)
        {
          m_drawCommand->updateCommandBuffer(commandIndex, meshIterator->second.size());
        }
      }

      m_matrixBuffer->resizeMatrixBuffer(m_instanceNumber);
      
      std::vector<GLuint> meshIndices(m_instanceNumber);
      std::vector<GLuint> materialIndexData(m_instanceNumber);

      unsigned int meshIndex = 0;
      unsigned int instanceIndex = 0;
      for(std::map<util::ResourceHandle, std::list<xBar::StaticGeometryContainer>, Less>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, meshIndex++)
      {
        for(std::list<xBar::StaticGeometryContainer>::const_iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, instanceIndex++)
        {
          materialIndexData[instanceIndex] = m_materialHandles[(*geometryIterator).getMaterialHandle()];
          meshIndices[instanceIndex] = meshIndex;
        }
      }

      m_materialIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_instanceNumber, 0, GL_STATIC_DRAW, &materialIndexData[0]);
      m_meshInstanceIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_instanceNumber, 0, GL_STATIC_DRAW, &meshIndices[0]);

      m_materialBuffer.uploadData();
    }
	}
}
