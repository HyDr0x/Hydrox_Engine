#include "Renderer/TreeNodes/RenderNodeDecorator/DrawElementsDecorator.h"

#include <XBar/StaticGeometryContainer.h>

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    DrawElementsDecorator::DrawElementsDecorator(IRenderNode *renderNode, bool instanced, GLenum indexType, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager) : 
      ARenderNodeDecorator(renderNode), 
      m_indexType(indexType), 
      m_primitiveType(primitiveType),
      m_vertexStride(vertexStride),
      m_vboSize(0),
      m_iboSize(0),
      m_meshNumberChanged(false)
    {
      m_modelManager = singletonManager->getService<ModelManager>();
    }

    DrawElementsDecorator::~DrawElementsDecorator()
    {
    }

    bool DrawElementsDecorator::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
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
          m_iboSize += mesh->getIndexCount() * sizeof(GLINDEXTYPE);
        }

        m_meshes[geometryContainer.getMeshHandle()].instanceNumber++;
        m_geometry.push_back(geometryContainer.getMeshHandle());

        return true;
      }

     return false;
    }

    unsigned int DrawElementsDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
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

        m_meshes[geometryContainer.getMeshHandle()].instanceNumber--;

        if(!m_meshes[geometryContainer.getMeshHandle()].instanceNumber)
        {
          m_meshNumberChanged = true;

          Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());

          m_vboSize -= mesh->getVBOSize();
          m_iboSize -= mesh->getIndexCount() * sizeof(GLuint);

          m_meshes.erase(geometryContainer.getMeshHandle());
        }
      }

      return instanceIndex;
    }

    void DrawElementsDecorator::frustumCulling()
    {
      m_bboxesBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_commandBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_meshInstanceBufferIndex.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

      m_renderNode->frustumCulling();

      m_meshInstanceBufferIndex.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_commandBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_bboxesBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    }

    void DrawElementsDecorator::rasterizeGeometry()
    {
      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      m_commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);
      m_meshIndexBuffer.bindBuffer(GL_ELEMENT_ARRAY_BUFFER);

      std::vector<DrawElementsIndirectCommand> tmpTest(getInstanceNumber());
      m_commandBuffer.getData(0, sizeof(DrawElementsIndirectCommand)* getInstanceNumber(), &tmpTest[0]);

      glMultiDrawElementsIndirect(m_primitiveType, m_indexType, nullptr, getInstanceNumber(), 0);

      m_meshIndexBuffer.unbindBuffer(GL_ELEMENT_ARRAY_BUFFER);
      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);
    }

    void DrawElementsDecorator::updateBuffer()
    {
      if (m_meshNumberChanged)
      {
        createBuffer();
        fillBuffer();

        m_meshVertexBuffer.syncWithFence();
        m_meshIndexBuffer.syncWithFence();
        m_bboxesBuffer.syncWithFence();
        m_commandBuffer.syncWithFence();
        m_meshInstanceBufferIndex.syncWithFence();

        m_meshNumberChanged = false;
      }
      //else if(hasInstanceNumberChanged())
      //{
      //  updateCommandBuffer();
      //}

      m_renderNode->updateBuffer();
    }

    void DrawElementsDecorator::createBuffer()
    {
      m_meshVertexBuffer.createBuffer(GL_ARRAY_BUFFER, m_vboSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_meshIndexBuffer.createBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);

      unsigned int instanceCount = getInstanceNumber();

      m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_meshes.size() * 2, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_meshInstanceBufferIndex.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint)* instanceCount, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand)* instanceCount, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    void DrawElementsDecorator::fillBuffer()
    {
      m_meshVertexBuffer.setMemoryFence();
      m_meshIndexBuffer.setMemoryFence();

      unsigned int bufferIndex = 0;
      unsigned int vertexOffset = 0;
      unsigned int indexOffset = 0;
      for(std::map<util::ResourceHandle, ElementGeometry, Less>::iterator meshIterator = m_meshes.begin(); meshIterator != m_meshes.end(); meshIterator++, bufferIndex++)
      {
        Mesh *mesh = m_modelManager->getObject(meshIterator->first);

        m_meshVertexBuffer.setData(vertexOffset * mesh->getVertexStride(), mesh->getVBOSize(), &(mesh->getVBOBuffer()[0]));
        m_meshIndexBuffer.setData(indexOffset * sizeof(GLINDEXTYPE), mesh->getIndexCount() * sizeof(GLINDEXTYPE), &(mesh->getIndexBuffer()[0]));

        meshIterator->second.bufferIndex = bufferIndex;
        meshIterator->second.vertexOffset = vertexOffset;
        meshIterator->second.indexOffset = indexOffset;

        vertexOffset += mesh->getVertexCount();
        indexOffset += mesh->getIndexCount();
      }

      m_bboxesBuffer.setMemoryFence();
      m_meshInstanceBufferIndex.setMemoryFence();
      m_commandBuffer.setMemoryFence();

      unsigned int instanceCounter = 0;
      for(std::list<util::ResourceHandle>::const_iterator meshIterator = m_geometry.begin(); meshIterator != m_geometry.end(); meshIterator++, instanceCounter++)
      {
        Mesh *mesh = m_modelManager->getObject(*meshIterator);

        DrawElementsIndirectCommand command;
        command.count = mesh->getIndexCount();
        command.instanceCount = 1;
        command.firstIndex = m_meshes[*meshIterator].indexOffset;
        command.baseVertex = m_meshes[*meshIterator].vertexOffset;
        command.baseInstance = instanceCounter;

        m_commandBuffer.setData(instanceCounter * sizeof(DrawElementsIndirectCommand), sizeof(DrawElementsIndirectCommand), &command);
        m_meshInstanceBufferIndex.setData(instanceCounter * sizeof(GLuint), sizeof(GLuint), &m_meshes[*meshIterator].bufferIndex);

        //update bbox data
        m_bboxesBuffer.setData((2 * m_meshes[*meshIterator].bufferIndex + 0) * sizeof(util::Vector<float, 4>), sizeof(util::Vector<float, 3>), &mesh->getBBMin()[0]);
        m_bboxesBuffer.setData((2 * m_meshes[*meshIterator].bufferIndex + 1) * sizeof(util::Vector<float, 4>), sizeof(util::Vector<float, 3>), &mesh->getBBMax()[0]);
      }
    }

    void DrawElementsDecorator::updateCommandBuffer()
    {
      unsigned int commandIndex = 0;
      for(std::map<util::ResourceHandle, ElementGeometry, Less>::const_iterator meshIterator = m_meshes.begin(); meshIterator != m_meshes.end(); meshIterator++, commandIndex++)
      {
        m_commandBuffer.setData(commandIndex * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &meshIterator->second);
      }
    }
  }
}