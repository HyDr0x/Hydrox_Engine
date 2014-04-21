#include "Renderer/TreeNodes/RenderNodeDecorator/DrawArrayDecorator.h"

#include <XBar/StaticGeometryContainer.h>

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    DrawArrayDecorator::DrawArrayDecorator(IRenderNode *renderNode, bool instanced, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager) : 
      ARenderNodeDecorator(renderNode), 
      m_primitiveType(primitiveType),
      m_vertexStride(vertexStride),
      m_vboSize(0),
      m_meshNumberChanged(false)
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
        if(!m_meshes.count(geometryContainer.getMeshHandle()))
        {
          m_meshNumberChanged = true;

          m_meshes[geometryContainer.getMeshHandle()].instanceCount = 0;

          m_vboSize += mesh->getVBOSize();
        }

        m_meshes[geometryContainer.getMeshHandle()].instanceCount++;
        m_geometry.push_back(geometryContainer.getMeshHandle());

        return true;
      }

     return false;
    }

    unsigned int DrawArrayDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      unsigned int instanceIndex = m_renderNode->removeGeometry(geometryContainer);
      if(instanceIndex != ~0)
      {
        unsigned int instanceCounter = 0;
        for(std::list<util::ResourceHandle>::const_iterator meshIterator = m_geometry.begin(); meshIterator != m_geometry.end(); meshIterator++, instanceCounter++)
        {
          if(instanceCounter == instanceIndex)
          {
            m_geometry.erase(meshIterator);
            break;
          }
        }

        m_meshes[geometryContainer.getMeshHandle()].instanceCount--;

        if(!m_meshes[geometryContainer.getMeshHandle()].instanceCount)
        {
          m_meshNumberChanged = true;

          Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());

          m_vboSize -= mesh->getVBOSize();

          m_meshes.erase(geometryContainer.getMeshHandle());
        }
      }

      return instanceIndex;
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

      glMultiDrawArraysIndirect(m_primitiveType, nullptr, getInstanceNumber(), sizeof(DrawArraysIndirectCommand));

      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);
    }

    void DrawArrayDecorator::updateBuffer()
    {
      if (m_meshNumberChanged)
      {
        createBuffer();
        fillBuffer();

        m_meshVertexBuffer.syncWithFence();
        m_bboxesBuffer.syncWithFence();
        m_commandBuffer.syncWithFence();
        m_meshInstanceIndexBuffer.syncWithFence();

        m_meshNumberChanged = false;
      }
      //else if(hasInstanceNumberChanged())
      //{
      //  updateCommandBuffer();
      //}

      m_renderNode->updateBuffer();
    }

    void DrawArrayDecorator::createBuffer()
    {
      m_meshVertexBuffer.createBuffer(GL_ARRAY_BUFFER, m_vboSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      
      unsigned int instanceCount = getInstanceNumber();

      m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_meshes.size() * 2, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_meshInstanceIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint)* instanceCount, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand)* instanceCount, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    void DrawArrayDecorator::fillBuffer()
    {
      m_meshVertexBuffer.setMemoryFence();

      unsigned int meshIndex = 0;
      unsigned int vertexOffset = 0;
      for(std::map<util::ResourceHandle, ArrayGeometry, Less>::iterator meshIterator = m_meshes.begin(); meshIterator != m_meshes.end(); meshIterator++, meshIndex++)
      {
        Mesh *mesh = m_modelManager->getObject(meshIterator->first);

        m_meshVertexBuffer.setData(vertexOffset * mesh->getVertexStride(), mesh->getVBOSize(), &(mesh->getVBOBuffer()[0]));

        meshIterator->second.meshIndex = meshIndex;
        meshIterator->second.vertexOffset = vertexOffset;

        vertexOffset += mesh->getVertexCount();
      }

      m_commandBuffer.setMemoryFence();
      m_bboxesBuffer.setMemoryFence();
      m_meshInstanceIndexBuffer.setMemoryFence();

      unsigned int instanceCounter = 0;
      for(std::list<util::ResourceHandle>::const_iterator meshIterator = m_geometry.begin(); meshIterator != m_geometry.end(); meshIterator++, instanceCounter++)
      {
        Mesh *mesh = m_modelManager->getObject(*meshIterator);

        DrawArraysIndirectCommand command;
        command.count = mesh->getVertexCount();
        command.instanceCount = 1;
        command.baseVertex = m_meshes[*meshIterator].vertexOffset;
        command.baseInstance = instanceCounter;

        m_commandBuffer.setData(sizeof(DrawArraysIndirectCommand)* instanceCounter, sizeof(DrawArraysIndirectCommand), &command);

        m_meshInstanceIndexBuffer.setData(sizeof(unsigned int)* instanceCounter, sizeof(unsigned int), &m_meshes[*meshIterator].meshIndex);

        //update bbox data
        m_bboxesBuffer.setData((2 * m_meshes[*meshIterator].meshIndex + 0) * sizeof(util::Vector<float, 4>), sizeof(util::Vector<float, 3>), &mesh->getBBMin()[0]);
        m_bboxesBuffer.setData((2 * m_meshes[*meshIterator].meshIndex + 1) * sizeof(util::Vector<float, 4>), sizeof(util::Vector<float, 3>), &mesh->getBBMax()[0]);
      }
    }

    void DrawArrayDecorator::updateCommandBuffer()
    {
      unsigned int commandIndex = 0;
      for(std::map<util::ResourceHandle, ArrayGeometry, Less>::const_iterator meshIterator = m_meshes.begin(); meshIterator != m_meshes.end(); meshIterator++, commandIndex++)
      {
        m_commandBuffer.setData(commandIndex * sizeof(DrawArraysIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &meshIterator->second);
      }
    }
  }
}