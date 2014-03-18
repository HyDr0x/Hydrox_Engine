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
      m_instanceNumber(0),
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

        m_instanceNumber++;
        m_instanceNumberPerMesh[geometryContainer.getMeshHandle()]++;

        return true;
      }

     return false;
    }

    bool DrawArrayDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_renderNode->removeGeometry(geometryContainer))
      {
        m_instanceNumber--;
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

    void DrawArrayDecorator::rasterizeGeometry()
    {
      m_commandBuffer.bindBuffer();
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);

      glMultiDrawArraysIndirect(m_primitiveType, nullptr, m_meshNumber, 0);

      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer();
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
      m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_meshNumber * 2, 0, GL_STATIC_DRAW, nullptr);

      if(m_instanced)
      {
        m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand) * m_meshNumber, 0, GL_STATIC_DRAW, nullptr);
        m_commandCache.resize(m_meshNumber);
      }
      else
      {
        m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand) * m_instanceNumber, 0, GL_STATIC_DRAW, nullptr);
        m_commandCache.resize(m_instanceNumber);
      }

      m_boundingBoxCache.resize(m_meshNumber * 2);

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
          }

          commandIndex += meshIterator->second;
        }

        m_vertexCount += mesh->getVertexCount();
        instanceCounter += meshIterator->second;

        //update bbox data
        memcpy(&m_boundingBoxCache[2 * meshIndex + 0][0], &mesh->getBBMin()[0], sizeof(util::Vector<float, 3>));
        memcpy(&m_boundingBoxCache[2 * meshIndex + 1][0], &mesh->getBBMax()[0], sizeof(util::Vector<float, 3>));
      }
    }

    void DrawArrayDecorator::fillBuffer()
    {
      m_bboxesBuffer.setData(0, sizeof(util::Vector<float, 4>) * m_meshNumber * 2, &m_boundingBoxCache[0]);

      if(m_instanced)
      {
        m_commandBuffer.setData(0, sizeof(DrawArraysIndirectCommand) * m_meshNumber, &m_commandCache[0]);
      }
      else
      {
        m_commandBuffer.setData(0, sizeof(DrawArraysIndirectCommand) * m_instanceNumber, &m_commandCache[0]);
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