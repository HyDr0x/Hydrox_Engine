#include "Renderer/TreeNodes/RenderNodeDecorator/DrawArrayDecorator.h"

#include <XBar/StaticGeometryContainer.h>

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    DrawArrayDecorator::DrawArrayDecorator(IRenderNode *renderNode, bool instanced, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager) : 
      ARenderNodeDecorator(renderNode), 
      m_instanced(instanced),
      m_primitiveType(primitiveType),
      m_vertexStride(vertexStride),
      m_meshNumber(0),
      m_vboSize(0)
    {
      m_modelManager = singletonManager->getService<ModelManager>();
    }

    DrawArrayDecorator::~DrawArrayDecorator()
    {
    }

    bool DrawArrayDecorator::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());

      if(m_primitiveType != mesh->getPrimitiveType())
      {
        return false;
      }

      if(m_renderNode->insertGeometry(geometryContainer))
      {
        if(m_instanceNumberPerMesh[geometryContainer.getMeshHandle()] == 0)
        {
          m_vboSize += mesh->getVBOSize();
        }

        m_instanceNumberPerMesh[geometryContainer.getMeshHandle()]++;

        return true;
      }

     return false;
    }

    bool DrawArrayDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_renderNode->removeGeometry(geometryContainer))
      {
        m_instanceNumberPerMesh[geometryContainer.getMeshHandle()]--;

        if(m_instanceNumberPerMesh[geometryContainer.getMeshHandle()] == 0)
        {
          m_instanceNumberPerMesh.erase(geometryContainer.getMeshHandle());

          Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());

          m_vboSize -= mesh->getVBOSize();
        }

        return true;
      }

      return false;
    }

    bool DrawArrayDecorator::isInstanced()
    {
      return m_instanced;
    }

    void DrawArrayDecorator::frustumCulling()
    {
      m_bboxesBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_commandBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_meshInstanceIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

      m_renderNode->frustumCulling();

      m_meshInstanceIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_commandBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_bboxesBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    }

    void DrawArrayDecorator::rasterizeGeometry()
    {
      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      m_commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);

      if(m_instanced)
      {
        glMultiDrawArraysIndirect(m_primitiveType, nullptr, m_meshNumber, sizeof(DrawArraysIndirectCommand));
      }
      else
      {
        glMultiDrawArraysIndirect(m_primitiveType, nullptr, getInstanceCount(), sizeof(DrawArraysIndirectCommand));
      }

      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);
    }

    void DrawArrayDecorator::updateBuffer()
    {
      if(hasGeometryChanged() || hasInstanceNumberChanged())
      {
        resizeBuffer();
      }

      m_renderNode->updateBuffer();
    }

    void DrawArrayDecorator::resizeBuffer()
    {
      m_meshNumber = m_instanceNumberPerMesh.size();

      if(hasGeometryChanged())
      {
        createBuffer();
        fillCaches();
        fillBuffer();
      }
      else if(hasInstanceNumberChanged())
      {
        updateCommandBuffer();
      }
    }

    void DrawArrayDecorator::createBuffer()
    {
      m_meshVertexBuffer.createBuffer(GL_ARRAY_BUFFER, m_vboSize, 0, GL_STATIC_DRAW, nullptr);
      
      if(m_instanced)
      {
        m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand) * m_meshNumber, 0, GL_STATIC_DRAW, nullptr);
        m_commandCache.resize(m_meshNumber);
      }
      else
      {
        unsigned int instanceCount = getInstanceCount();

        m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_meshNumber * 2, 0, GL_STATIC_DRAW, nullptr);
        m_meshInstanceIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * instanceCount, 0, GL_STATIC_DRAW, nullptr);
        m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand) * instanceCount, 0, GL_STATIC_DRAW, nullptr);
        
        m_boundingBoxCache.resize(m_meshNumber * 2);
        m_meshInstanceIndexCache.resize(instanceCount);
        m_commandCache.resize(instanceCount);
      }

      m_vertexOffset = 0;
      m_vertexCount = 0;
    }

    void DrawArrayDecorator::fillCaches()
    {
      unsigned int instanceCounter = 0;
      unsigned int commandIndex = 0;
      unsigned int meshIndex = 0;
      for(std::map<util::ResourceHandle, unsigned int, Less>::const_iterator meshIterator = m_instanceNumberPerMesh.begin(); meshIterator != m_instanceNumberPerMesh.end(); meshIterator++, meshIndex++)
      {
        Mesh *mesh = m_modelManager->getObject(meshIterator->first);

        m_meshVertexBuffer.setData(m_vertexOffset, mesh->getVBOSize(), &(mesh->getVBOBuffer()[0]));
        m_vertexOffset += mesh->getVBOSize();

        if(m_instanced)
        {
          DrawArraysIndirectCommand command;
          command.count = mesh->getVertexCount();
          command.instanceCount = meshIterator->second;
          command.baseVertex = m_vertexCount;
          command.baseInstance = instanceCounter;

          m_commandCache[commandIndex] = command;

          commandIndex++;
        }
        else
        {
          for(unsigned int i = 0; i < meshIterator->second; i++)
          {
            DrawArraysIndirectCommand command;
            command.count = mesh->getVertexCount();
            command.instanceCount = 1;
            command.baseVertex = m_vertexCount;
            command.baseInstance = instanceCounter + i;

            m_commandCache[commandIndex + i] = command;
            m_meshInstanceIndexCache[commandIndex + i] = meshIndex;
          }

          commandIndex += meshIterator->second;

          //update bbox data
          memcpy(&m_boundingBoxCache[2 * meshIndex + 0][0], &mesh->getBBMin()[0], sizeof(util::Vector<float, 3>));
          memcpy(&m_boundingBoxCache[2 * meshIndex + 1][0], &mesh->getBBMax()[0], sizeof(util::Vector<float, 3>));
        }

        m_vertexCount += mesh->getVertexCount();
        instanceCounter += meshIterator->second;
      }
    }

    void DrawArrayDecorator::fillBuffer()
    {
      if(m_instanced)
      {
        m_commandBuffer.setData(0, sizeof(DrawArraysIndirectCommand) * m_meshNumber, &m_commandCache[0]);
      }
      else
      {
        unsigned int instanceCount = getInstanceCount();

        m_bboxesBuffer.setData(0, sizeof(util::Vector<float, 4>) * m_meshNumber * 2, &m_boundingBoxCache[0]);
        m_meshInstanceIndexBuffer.setData(0, sizeof(GLuint) * instanceCount, &m_meshInstanceIndexCache[0]);
        m_commandBuffer.setData(0, sizeof(DrawArraysIndirectCommand) * instanceCount, &m_commandCache[0]);
      }
    }

    void DrawArrayDecorator::updateCommandBuffer()
    {
      unsigned int commandIndex = 0;
      for(std::map<util::ResourceHandle, unsigned int, Less>::const_iterator meshIterator = m_instanceNumberPerMesh.begin(); meshIterator != m_instanceNumberPerMesh.end(); meshIterator++, commandIndex++)
      {
        m_commandBuffer.setData(commandIndex * sizeof(DrawArraysIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &meshIterator->second);
      }
    }
  }
}