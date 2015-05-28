#include "Renderer/TreeNodes/RenderNodeDecorator/DrawArrayDecorator.h"

#include <XBar/IGeometryContainer.h>

#include <DataBase/Mesh.h>

namespace he
{
  namespace renderer
  {
    DrawArrayDecorator::DrawArrayDecorator(IRenderGroup *renderNode, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager) :
      ARenderNodeDecorator(renderNode), 
      m_primitiveType(primitiveType),
      m_vertexStride(vertexStride),
      m_cacheNumber(0),
      m_perInstanceCacheNumber(0),
      m_updateMeshData(false)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_options = singletonManager->getService<RenderOptions>();

      m_meshVertexBuffer.createBuffer(GL_ARRAY_BUFFER, m_options->vboBlockSize * m_vertexStride, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::vec4f) * 2 * m_options->perMeshBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_triangleIndexOffsetBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_options->perMeshBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_cacheOffsetBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_options->perMeshBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_triangleBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::vec2ui) * m_options->triangleBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_cacheBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Cache) * m_options->cacheBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);

      m_cacheInstanceOffsetBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_options->perInstanceBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_meshInstanceBufferIndex.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_options->perInstanceBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand) * m_options->perInstanceBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    DrawArrayDecorator::~DrawArrayDecorator()
    {
    }

    bool DrawArrayDecorator::insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());

      if(m_primitiveType != mesh->getPrimitiveType())
      {
        return false;
      }

      if(m_renderNode->insertGeometry(geometryContainer))
      {
        if(!m_meshes.count(geometryContainer->getMeshHandle()))
        {
          m_updateMeshData = true;

          m_meshes[geometryContainer->getMeshHandle()].instanceNumber = 0;
          m_meshHashes[geometryContainer->getMeshHandle()] = mesh->getHash();
        }

        m_meshes[geometryContainer->getMeshHandle()].instanceNumber++;

        return true;
      }

     return false;
    }

    bool DrawArrayDecorator::removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      bool deleted = m_renderNode->removeGeometry(geometryContainer);
      if(deleted)
      {
        db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());

        m_meshes[geometryContainer->getMeshHandle()].instanceNumber--;

        if(!m_meshes[geometryContainer->getMeshHandle()].instanceNumber)
        {
          m_updateMeshData = true;

          m_meshes.erase(geometryContainer->getMeshHandle());
          m_meshHashes.erase(geometryContainer->getMeshHandle());
        }
      }

      return deleted;
    }

    void DrawArrayDecorator::frustumCulling() const
    {
      m_bboxesBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_commandBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_meshInstanceBufferIndex.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

      m_renderNode->frustumCulling();

      m_meshInstanceBufferIndex.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_commandBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_bboxesBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    }

    void DrawArrayDecorator::rasterizeShadowGeometry() const
    {
      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      m_commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);

      glMultiDrawArraysIndirect(m_primitiveType, nullptr, getInstanceNumber(), sizeof(DrawArraysIndirectCommand));

      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);
    }

    void DrawArrayDecorator::rasterizeReflectiveShadowGeometry() const
    {
      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      m_commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);

      glMultiDrawArraysIndirect(m_primitiveType, nullptr, getInstanceNumber(), sizeof(DrawArraysIndirectCommand));

      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);
    }

    void DrawArrayDecorator::rasterizeIndexGeometry() const
    {
      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      m_commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);
      m_triangleIndexOffsetBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheOffsetBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_triangleBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_cacheBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 5);
      m_meshInstanceBufferIndex.bindBuffer(GL_SHADER_STORAGE_BUFFER, 6);
      m_cacheInstanceOffsetBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 7);

      glMultiDrawArraysIndirect(m_primitiveType, nullptr, getInstanceNumber(), sizeof(DrawArraysIndirectCommand));

      m_cacheInstanceOffsetBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 7);
      m_meshInstanceBufferIndex.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 6);
      m_cacheBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 5);
      m_triangleBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_cacheOffsetBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_triangleIndexOffsetBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);
    }

    void DrawArrayDecorator::rasterizeIndirectLightingGeometry() const
    {
      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      m_commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);
      m_cacheInstanceOffsetBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      glMultiDrawArraysIndirect(m_primitiveType, nullptr, getInstanceNumber(), sizeof(DrawArraysIndirectCommand));

      m_cacheInstanceOffsetBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);
    }

    void DrawArrayDecorator::rasterizeGeometry() const
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
      for(std::map<util::ResourceHandle, uint64_t, util::ResourceHandle::Less>::iterator meshIterator = m_meshHashes.begin(); meshIterator != m_meshHashes.end(); meshIterator++)
      {
        db::Mesh *mesh = m_modelManager->getObject(meshIterator->first);

        if(mesh->getHash() != meshIterator->second)
        {
          m_updateMeshData = true;//we need to update the mesh data
          break;
        }
      }

      if(m_updateMeshData)
      {
        updatePerMeshBuffer();

        m_meshVertexBuffer.syncWithFence();
        m_bboxesBuffer.syncWithFence();
        m_triangleIndexOffsetBuffer.syncWithFence();
        m_cacheOffsetBuffer.syncWithFence();
        m_triangleBuffer.syncWithFence();
        m_cacheBuffer.syncWithFence();
      }
      
      if(hasInstanceNumberChanged() || m_updateMeshData)
      {
        updatePerInstanceBuffer();

        m_commandBuffer.syncWithFence();
        m_meshInstanceBufferIndex.syncWithFence();
        m_cacheInstanceOffsetBuffer.syncWithFence();
      }

      m_updateMeshData = false;

      m_renderNode->updateBuffer();
    }

    unsigned int DrawArrayDecorator::getCacheNumber() const
    {
      return m_perInstanceCacheNumber;
    }

    void DrawArrayDecorator::updatePerMeshBuffer()
    {
      m_meshVertexBuffer.setMemoryFence();
      m_bboxesBuffer.setMemoryFence();
      m_triangleIndexOffsetBuffer.setMemoryFence();
      m_cacheOffsetBuffer.setMemoryFence();
      m_triangleBuffer.setMemoryFence();
      m_cacheBuffer.setMemoryFence();

      unsigned int bufferIndex = 0;
      unsigned int vertexOffset = 0;
      unsigned int triangleOffset = 0;
      unsigned int cacheOffset = 0;
      for(std::map<util::ResourceHandle, ArrayGeometry, util::ResourceHandle::Less>::iterator meshIterator = m_meshes.begin(); meshIterator != m_meshes.end(); meshIterator++, bufferIndex++)
      {
        db::Mesh *mesh = m_modelManager->getObject(meshIterator->first);

        m_meshVertexBuffer.setData(vertexOffset * mesh->getVertexStride(), mesh->getVBOSize(), &mesh->getVBOBuffer()[0]);
        m_triangleIndexOffsetBuffer.setData(bufferIndex * sizeof(GLuint), sizeof(GLuint), &triangleOffset);
        m_cacheOffsetBuffer.setData(bufferIndex * sizeof(GLuint), sizeof(GLuint), &cacheOffset);
        if(!mesh->getTriangleCacheIndices().empty()) m_triangleBuffer.setData(triangleOffset * sizeof(util::vec2ui), mesh->getTriangleCacheIndices().size() * sizeof(util::vec2ui), &mesh->getTriangleCacheIndices()[0]);
        if(mesh->getCacheCount()) m_cacheBuffer.setData(cacheOffset * sizeof(util::Cache), mesh->getCacheCount() * sizeof(util::Cache), &mesh->getCaches()[0]);

        meshIterator->second.bufferIndex = bufferIndex;
        meshIterator->second.vertexOffset = vertexOffset;

        vertexOffset += mesh->getVertexCount();
        triangleOffset += mesh->getPrimitiveCount();
        cacheOffset += mesh->getCacheCount();

        m_meshHashes[meshIterator->first] = mesh->getHash();

        //update bbox data
        m_bboxesBuffer.setData((2 * meshIterator->second.bufferIndex + 0) * sizeof(util::vec4f), sizeof(util::vec3f), &mesh->getBBMin()[0]);
        m_bboxesBuffer.setData((2 * meshIterator->second.bufferIndex + 1) * sizeof(util::vec4f), sizeof(util::vec3f), &mesh->getBBMax()[0]);
      }

      m_cacheNumber = cacheOffset;
    }

    void DrawArrayDecorator::updatePerInstanceBuffer()
    {
      unsigned int instanceCount = getInstanceNumber();

      m_cacheInstanceOffsetBuffer.setMemoryFence();
      m_meshInstanceBufferIndex.setMemoryFence();
      m_commandBuffer.setMemoryFence();

      unsigned int perInstanceCacheOffsetCounter = 0;
      unsigned int instanceCounter = 0;
      resetInstanceIterator();
      while(!isEndInstanceIterator())
      {
        const xBar::IGeometryContainer& instance = incInstanceIterator();
        db::Mesh *mesh = m_modelManager->getObject(instance.getMeshHandle());

        DrawArraysIndirectCommand command;
        command.count = mesh->getVertexCount();
        command.instanceCount = 1;
        command.baseVertex = m_meshes[instance.getMeshHandle()].vertexOffset;
        command.baseInstance = instanceCounter;

        m_cacheInstanceOffsetBuffer.setData(sizeof(GLuint) * instanceCounter, sizeof(GLuint), &perInstanceCacheOffsetCounter);
        m_commandBuffer.setData(sizeof(DrawArraysIndirectCommand) * instanceCounter, sizeof(DrawArraysIndirectCommand), &command);
        m_meshInstanceBufferIndex.setData(sizeof(unsigned int) * instanceCounter, sizeof(unsigned int), &m_meshes[instance.getMeshHandle()].bufferIndex);

        perInstanceCacheOffsetCounter += mesh->getCacheCount();
        instanceCounter++;
      }

      m_perInstanceCacheNumber = perInstanceCacheOffsetCounter;
    }
  }
}