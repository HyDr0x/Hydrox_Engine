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
      m_instanceCount(0), 
      m_geometryCounter(0), 
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

    void RenderNode::initialize(MaterialManager *materialManager, ModelManager *modelManager, util::ResourceHandle cullingShaderHandle)
    {
      m_materialManager = materialManager;
      m_modelManager = modelManager;

      //m_frustumCulling.initialize(computeShaderManager, cullingShaderHandle);
    }

    bool RenderNode::insertGeometry(sg::GeoNode *node)
    {
      Mesh *mesh = m_modelManager->getObject(node->getMeshHandle());

      if(m_primitiveType == ~0)
      {
        m_primitiveType = mesh->getPrimitiveType();
      }

      if(m_geometryCounter >= m_maxGeometry || m_primitiveType != mesh->getPrimitiveType())
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

      if(m_meshHandles[mesh].size() != 0)
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

      m_meshHandles[mesh].push_back(node);
      m_geometryCounter++;

      return true;
    }

    bool RenderNode::removeGeometry(sg::GeoNode *node)
    {
      Mesh *mesh = m_modelManager->getObject(node->getMeshHandle());
      m_meshHandles[mesh].remove(node);

      m_instanceNumberChanged = true;

      if(m_meshHandles[mesh].size() == 0)
      {
        m_geometryChanged = true;

        m_meshHandles.erase(mesh);

        m_vboSize -= mesh->getVBOSize();
        m_indexSize -= mesh->getIndexCount() * sizeof(GLuint);
      }

      m_geometryCounter--;

      return m_meshHandles[mesh].size() == 0;
    }

    void RenderNode::rasterizeGeometry()
    {
      if(m_geometryChanged || m_instanceNumberChanged)
      {
        resizeBuffer();

        m_geometryChanged = false;
        m_instanceNumberChanged = false;
      }

      Mesh *renderMesh;

      //std::vector<util::Matrix<float, 4>> transformMatrices(m_geoNodes.size());
      //std::vector<util::Vector<float, 4>> bbMin(m_geoNodes.size());
      //std::vector<util::Vector<float, 4>> bbMax(m_geoNodes.size());

      //unsigned int k = 0;
      //for(std::list<sg::GeoNode*>::const_iterator geometryIterator = m_geoNodes.begin(); geometryIterator != m_geoNodes.end(); geometryIterator++, k++)//Render 3D Objects
      //{
      //  renderMesh = m_modelManager->getObject((*geometryIterator)->getMeshHandle());//SAVE ALLE MESHES, NO NEED TO CALL IT THEN ANYMORE, REPLACE THE OBSERVER THROUGH A ONE ELEMENT 

      //  transformMatrices[k] = (*geometryIterator)->getTransformationMatrix();
      //  memcpy(&bbMin[k][0], &renderMesh->getBBMin()[0], sizeof(util::Vector<float, 3>));//USE INSTANCING HERE, NEED BBOXES ONLY ONCE PER MESH
      //  memcpy(&bbMax[k][0], &renderMesh->getBBMax()[0], sizeof(util::Vector<float, 3>));
      //}

      //std::vector<unsigned int> culledAABB;
      //culledAABB.resize(m_geoNodes.size());
      //m_frustumCulling.cullAABB(transformMatrices, bbMin, bbMax, culledAABB);

      unsigned int geometryCounter = 0;
      for(std::map<Mesh*, std::list<sg::GeoNode*>>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++)
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

      m_matrixBuffer.bindBuffer(0);
      m_materialIndexBuffer.bindBuffer(1);

      m_materialBuffer.bindBuffer(1);

      m_commandBuffer.bindBuffer();
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);
      m_instanceIndexBuffer.bindVertexbuffer(1, 0, sizeof(GLuint));
      m_meshIndexBuffer.bindBuffer();

      glMultiDrawElementsIndirect(GL_TRIANGLES, GLINDEXTYPE, nullptr, m_meshHandles.size(), 0);

      m_meshIndexBuffer.unbindBuffer();
      m_instanceIndexBuffer.unbindVertexBuffer(1);
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

        unsigned int commandCounter = 0;
        unsigned int vertexOffset = 0;
        unsigned int indexOffset = 0;
        unsigned int vertexCount = 0;
        unsigned int indexCount = 0;
        unsigned int instanceCounter = 0;
        for(std::map<Mesh*, std::list<sg::GeoNode*>>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, commandCounter++)
        {
          m_meshVertexBuffer.setData(vertexOffset, meshIterator->first->getVBOSize(), &(meshIterator->first->getVBOBuffer()[0]));
          vertexOffset += meshIterator->first->getVBOSize();

          m_meshIndexBuffer.setData(indexOffset, meshIterator->first->getIndexCount() * sizeof(GLuint), &(meshIterator->first->getIndexBuffer()[0]));
          indexOffset += meshIterator->first->getIndexCount() * sizeof(GLuint);

          DrawElementsIndirectCommand command;
          command.count = meshIterator->first->getIndexCount();
          command.instanceCount = meshIterator->second.size();
          command.firstIndex = indexCount;
          command.baseVertex = vertexCount;
          command.baseInstance = instanceCounter;

          m_commandBuffer.setData(commandCounter * sizeof(DrawElementsIndirectCommand), sizeof(DrawElementsIndirectCommand), &command);

          indexCount += meshIterator->first->getIndexCount();
          vertexCount += meshIterator->first->getVertexCount();
          instanceCounter += meshIterator->second.size();
        }

        m_instanceCount = instanceCounter;
      }
      else if(m_instanceNumberChanged)
      {
        unsigned int commandCounter = 0;
        for(std::map<Mesh*, std::list<sg::GeoNode*>>::const_iterator commandIterator = m_meshHandles.begin(); commandIterator != m_meshHandles.end(); commandIterator++, commandCounter++)
        {
          size_t size = commandIterator->second.size();
          m_commandBuffer.setData(commandCounter * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &size);
        }
      }

      resizeMatrixBuffer();
      
      std::vector<GLuint> materialIndexData(m_instanceCount);
      std::vector<GLuint> instanceIndex(m_instanceCount);

      unsigned int geometryCounter = 0;
      for(std::map<Mesh*, std::list<sg::GeoNode*>>::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++)
      {
        for(std::list<sg::GeoNode*>::const_iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, geometryCounter++)
        {
          materialIndexData[geometryCounter] = m_materialHandles[(*geometryIterator)->getMaterialHandle().getID()];
          instanceIndex[geometryCounter] = geometryCounter;
        }
      }

      m_materialIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_instanceCount, 0, GL_STATIC_DRAW, &materialIndexData[0]);
      m_instanceIndexBuffer.createBuffer(GL_ARRAY_BUFFER, sizeof(GLuint) * m_instanceCount, 0, GL_STATIC_DRAW, &instanceIndex[0]);

      m_materialBuffer.uploadData();
    }
	}
}
