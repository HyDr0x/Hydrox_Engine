#include "Renderer/TreeNodes/RenderNodeDecorator/DrawElementsDecorator.h"

#include <XBar/StaticGeometryContainer.h>

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    DrawElementsDecorator::DrawElementsDecorator(IRenderNode *renderNode, GLenum indexType, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager) : 
      ARenderNodeDecorator(renderNode), 
      m_indexType(indexType), 
      m_primitiveType(primitiveType),
      m_vertexStride(vertexStride),
      m_vboSize(0),
      m_iboSize(0)
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
        if(m_instanceNumberPerMesh[geometryContainer.getMeshHandle()])
        {
          
        }
        else if(m_instanceNumberPerMesh[geometryContainer.getMeshHandle()] == 0)
        {
          m_vboSize += mesh->getVBOSize();
          m_iboSize += mesh->getIndexCount() * sizeof(GLuint);
        }

        m_instanceNumberPerMesh[geometryContainer.getMeshHandle()]++;

        return true;
      }

     return false;
    }

    bool DrawElementsDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_renderNode->removeGeometry(geometryContainer))
      {
        m_instanceNumberPerMesh[geometryContainer.getMeshHandle()]--;

        if(m_instanceNumberPerMesh[geometryContainer.getMeshHandle()] == 0)
        {
          m_instanceNumberPerMesh.erase(geometryContainer.getMeshHandle());

          Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());

          m_vboSize -= mesh->getVBOSize();
          m_iboSize -= mesh->getIndexCount() * sizeof(GLuint);
        }

        return true;
      }

      return false;
    }

    void DrawElementsDecorator::rasterizeGeometry()
    {
      m_commandBuffer.bindBuffer();
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);
      m_meshIndexBuffer.bindBuffer();

      glMultiDrawElementsIndirect(m_primitiveType, m_indexType, nullptr, m_primitiveCount, 0);

      m_meshIndexBuffer.unbindBuffer();
      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer();
    }

    void DrawElementsDecorator::updateBuffer()
    {
      if(hasGeometryChanged() || hasInstanceNumberChanged())
      {
        resizeBuffer();
      }

      m_renderNode->updateBuffer();
    }

    void DrawElementsDecorator::resizeBuffer()
    {
      m_primitiveCount = m_instanceNumberPerMesh.size();

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

    void DrawElementsDecorator::createBuffer()
    {
      m_meshVertexBuffer.createBuffer(GL_ARRAY_BUFFER, m_vboSize, 0, GL_STATIC_DRAW, nullptr);
      m_cullingCommandBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_primitiveCount, 0, GL_STATIC_DRAW, nullptr);
      m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_primitiveCount * 2, 0, GL_STATIC_DRAW, nullptr);
      m_meshIndexBuffer.createBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSize, 0, GL_STATIC_DRAW, nullptr);
      m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * m_primitiveCount, 0, GL_STATIC_DRAW, nullptr);

      m_commandCache.resize(m_primitiveCount);
      m_cullingCommandCache.resize(m_primitiveCount);
      m_boundingBoxCache.resize(m_primitiveCount * 2);

      m_vertexOffset = 0;
      m_indexOffset = 0;
      m_vertexCount = 0;
      m_indexCount = 0;
    }

    void DrawElementsDecorator::fillCaches()
    {
      unsigned int instanceCounter = 0;
      unsigned int commandIndex = 0;
      for(std::map<util::ResourceHandle, unsigned int, Less>::const_iterator meshIterator = m_instanceNumberPerMesh.begin(); meshIterator != m_instanceNumberPerMesh.end(); meshIterator++, commandIndex++)
      {
        Mesh *mesh = m_modelManager->getObject(meshIterator->first);

        m_meshVertexBuffer.setData(m_vertexOffset, mesh->getVBOSize(), &(mesh->getVBOBuffer()[0]));
        m_vertexOffset += mesh->getVBOSize();

        m_meshIndexBuffer.setData(m_indexOffset, mesh->getIndexCount() * sizeof(GLuint), &(mesh->getIndexBuffer()[0]));
        m_indexOffset += mesh->getIndexCount() * sizeof(GLuint);

        DrawElementsIndirectCommand command;
        command.count = mesh->getIndexCount();
        command.instanceCount = meshIterator->second;
        command.firstIndex = m_indexCount;
        command.baseVertex = m_vertexCount;
        command.baseInstance = instanceCounter;

        m_commandCache[commandIndex] = command;

        m_cullingCommandCache[commandIndex][0] = command.count;
        m_cullingCommandCache[commandIndex][1] = command.firstIndex;
        m_cullingCommandCache[commandIndex][2] = command.baseVertex;
        m_cullingCommandCache[commandIndex][3] = command.baseInstance;

        m_indexCount += mesh->getIndexCount();
        m_vertexCount += mesh->getVertexCount();
        instanceCounter += meshIterator->second;

        //update bbox data
        memcpy(&m_boundingBoxCache[2 * commandIndex + 0][0], &mesh->getBBMin()[0], sizeof(util::Vector<float, 3>));
        memcpy(&m_boundingBoxCache[2 * commandIndex + 1][0], &mesh->getBBMax()[0], sizeof(util::Vector<float, 3>));
      }
    }

    void DrawElementsDecorator::fillBuffer()
    {
      m_bboxesBuffer.setData(0, sizeof(util::Vector<float, 4>) * m_primitiveCount * 2, &m_boundingBoxCache[0]);
      m_cullingCommandBuffer.setData(0, sizeof(util::Vector<float, 4>) * m_primitiveCount, &m_cullingCommandCache[0]);
      m_commandBuffer.setData(0, sizeof(DrawElementsIndirectCommand) * m_primitiveCount, &m_commandCache[0]);
    }

    void DrawElementsDecorator::updateCommandBuffer()
    {
      unsigned int commandIndex = 0;
      for(std::map<util::ResourceHandle, unsigned int, Less>::const_iterator meshIterator = m_instanceNumberPerMesh.begin(); meshIterator != m_instanceNumberPerMesh.end(); meshIterator++, commandIndex++)
      {
        m_commandBuffer.setData(commandIndex * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &meshIterator->second);
      }
    }
  }
}