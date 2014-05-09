#include "Renderer/TreeNodes/RenderNodeDecorator/DrawArrayDecorator.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    DrawArrayDecorator::DrawArrayDecorator(IRenderNode *renderNode, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager) : 
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

    bool DrawArrayDecorator::insertGeometry(xBar::SkinnedGeometryContainer& geometryContainer)
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

          m_meshes[geometryContainer.getMeshHandle()].instanceNumber = 0;

          m_vboSize += mesh->getVBOSize();
        }

        m_meshes[geometryContainer.getMeshHandle()].instanceNumber++;

        return true;
      }

     return false;
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

          m_meshes[geometryContainer.getMeshHandle()].instanceNumber = 0;

          m_vboSize += mesh->getVBOSize();
        }

        m_meshes[geometryContainer.getMeshHandle()].instanceNumber++;

        return true;
      }

     return false;
    }

    bool DrawArrayDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      bool deleted = m_renderNode->removeGeometry(geometryContainer);
      if(deleted)
      {
        m_meshes[geometryContainer.getMeshHandle()].instanceNumber--;

        if(!m_meshes[geometryContainer.getMeshHandle()].instanceNumber)
        {
          m_meshNumberChanged = true;

          Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());

          m_vboSize -= mesh->getVBOSize();

          m_meshes.erase(geometryContainer.getMeshHandle());
        }
      }

      return deleted;
    }

    void DrawArrayDecorator::frustumCulling()
    {
      m_bboxesBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_commandBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_meshInstanceBufferIndex.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

      m_renderNode->frustumCulling();

      m_meshInstanceBufferIndex.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
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
      if(m_meshNumberChanged)
      {
        updatePerMeshBuffer();

        m_meshVertexBuffer.syncWithFence();
        m_bboxesBuffer.syncWithFence();

        m_meshNumberChanged = false;
      }
      
      if(hasInstanceNumberChanged())
      {
        updatePerInstanceBuffer();

        m_commandBuffer.syncWithFence();
        m_meshInstanceBufferIndex.syncWithFence();
      }

      m_renderNode->updateBuffer();
    }

    void DrawArrayDecorator::updatePerMeshBuffer()
    {
      m_meshVertexBuffer.createBuffer(GL_ARRAY_BUFFER, m_vboSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_meshes.size() * 2, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);

      m_meshVertexBuffer.setMemoryFence();
      m_bboxesBuffer.setMemoryFence();

      unsigned int bufferIndex = 0;
      unsigned int vertexOffset = 0;
      for(std::map<util::ResourceHandle, ArrayGeometry, Less>::iterator meshIterator = m_meshes.begin(); meshIterator != m_meshes.end(); meshIterator++, bufferIndex++)
      {
        Mesh *mesh = m_modelManager->getObject(meshIterator->first);

        m_meshVertexBuffer.setData(vertexOffset * mesh->getVertexStride(), mesh->getVBOSize(), &mesh->getVBOBuffer()[0]);

        meshIterator->second.bufferIndex = bufferIndex;
        meshIterator->second.vertexOffset = vertexOffset;

        vertexOffset += mesh->getVertexCount();

        //update bbox data
        m_bboxesBuffer.setData((2 * meshIterator->second.bufferIndex + 0) * sizeof(util::Vector<float, 4>), sizeof(util::Vector<float, 3>), &mesh->getBBMin()[0]);
        m_bboxesBuffer.setData((2 * meshIterator->second.bufferIndex + 1) * sizeof(util::Vector<float, 4>), sizeof(util::Vector<float, 3>), &mesh->getBBMax()[0]);
      }
    }

    void DrawArrayDecorator::updatePerInstanceBuffer()
    {
      unsigned int instanceCount = getInstanceNumber();
      m_meshInstanceBufferIndex.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint)* instanceCount, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand)* instanceCount, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);

      m_commandBuffer.setMemoryFence();
      m_meshInstanceBufferIndex.setMemoryFence();

      unsigned int instanceCounter = 0;
      for(std::list<xBar::StaticGeometryContainer*>::const_iterator instanceIterator = getInstances().begin(); instanceIterator != getInstances().end(); instanceIterator++, instanceCounter++)
      {
        Mesh *mesh = m_modelManager->getObject((*instanceIterator)->getMeshHandle());

        DrawArraysIndirectCommand command;
        command.count = mesh->getVertexCount();
        command.instanceCount = 1;
        command.baseVertex = m_meshes[(*instanceIterator)->getMeshHandle()].vertexOffset;
        command.baseInstance = instanceCounter;

        m_commandBuffer.setData(sizeof(DrawArraysIndirectCommand) * instanceCounter, sizeof(DrawArraysIndirectCommand), &command);

        m_meshInstanceBufferIndex.setData(sizeof(unsigned int) * instanceCounter, sizeof(unsigned int), &m_meshes[(*instanceIterator)->getMeshHandle()].bufferIndex);
      }
    }
  }
}