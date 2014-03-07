#include "Renderer/Pipeline/IndexedDrawCommand.h"

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    IndexedDrawCommand::IndexedDrawCommand(GLenum primitiveType, GLuint vertexStride, GLenum indexType) : DrawCommandInterface(primitiveType, vertexStride), m_indexType(indexType)
    {
    }

    IndexedDrawCommand::~IndexedDrawCommand()
    {
    }

    void IndexedDrawCommand::multiDraw()
    {
      m_commandBuffer.bindBuffer();
      m_meshVertexBuffer.bindVertexbuffer(0, 0, m_vertexStride);
      m_meshIndexBuffer.bindBuffer();

      glMultiDrawElementsIndirect(m_primitiveType, m_indexType, nullptr, m_primitiveCount, 0);

      m_meshIndexBuffer.unbindBuffer();
      m_meshVertexBuffer.unbindVertexBuffer(0);
      m_commandBuffer.unbindBuffer();
    }

    void IndexedDrawCommand::createBuffer(unsigned int primCount, unsigned int vboSize, unsigned int iboSize)
    {
      m_primitiveCount = primCount;

      m_meshVertexBuffer.createBuffer(GL_ARRAY_BUFFER, vboSize, 0, GL_STATIC_DRAW, nullptr);
      m_cullingCommandBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_primitiveCount, 0, GL_STATIC_DRAW, nullptr);
      m_bboxesBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Vector<float, 4>) * m_primitiveCount * 2, 0, GL_STATIC_DRAW, nullptr);
      m_meshIndexBuffer.createBuffer(GL_ELEMENT_ARRAY_BUFFER, iboSize, 0, GL_STATIC_DRAW, nullptr);
      m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * m_primitiveCount, 0, GL_STATIC_DRAW, nullptr);

      m_commandCache.resize(m_primitiveCount);
      m_cullingCommandCache.resize(m_primitiveCount);
      m_boundingBoxCache.resize(m_primitiveCount * 2);

      m_vertexOffset = 0;
      m_indexOffset = 0;
      m_vertexCount = 0;
      m_indexCount = 0;
      m_instanceCounter = 0;
    }

    void IndexedDrawCommand::fillCaches(unsigned int commandIndex, unsigned int instanceCount, Mesh *mesh)
    {
      m_meshVertexBuffer.setData(m_vertexOffset, mesh->getVBOSize(), &(mesh->getVBOBuffer()[0]));
      m_vertexOffset += mesh->getVBOSize();

      m_meshIndexBuffer.setData(m_indexOffset, mesh->getIndexCount() * sizeof(GLuint), &(mesh->getIndexBuffer()[0]));
      m_indexOffset += mesh->getIndexCount() * sizeof(GLuint);

      DrawElementsIndirectCommand command;
      command.count = mesh->getIndexCount();
      command.instanceCount = instanceCount;
      command.firstIndex = m_indexCount;
      command.baseVertex = m_vertexCount;
      command.baseInstance = m_instanceCounter;

      m_commandCache[commandIndex] = command;

      m_cullingCommandCache[commandIndex][0] = command.count;
      m_cullingCommandCache[commandIndex][1] = command.firstIndex;
      m_cullingCommandCache[commandIndex][2] = command.baseVertex;
      m_cullingCommandCache[commandIndex][3] = command.baseInstance;

      m_indexCount += mesh->getIndexCount();
      m_vertexCount += mesh->getVertexCount();
      m_instanceCounter += instanceCount;

      //update bbox data
      memcpy(&m_boundingBoxCache[2 * commandIndex + 0][0], &mesh->getBBMin()[0], sizeof(util::Vector<float, 3>));
      memcpy(&m_boundingBoxCache[2 * commandIndex + 1][0], &mesh->getBBMax()[0], sizeof(util::Vector<float, 3>));
    }

    void IndexedDrawCommand::fillBuffer()
    {
      m_bboxesBuffer.setData(0, sizeof(util::Vector<float, 4>) * m_primitiveCount * 2, &m_boundingBoxCache[0]);
      m_cullingCommandBuffer.setData(0, sizeof(util::Vector<float, 4>) * m_primitiveCount, &m_cullingCommandCache[0]);
      m_commandBuffer.setData(0, sizeof(DrawElementsIndirectCommand) * m_primitiveCount, &m_commandCache[0]);
    }

    void IndexedDrawCommand::updateCommandBuffer(unsigned int commandIndex, unsigned int instanceCount)
    {
      m_commandBuffer.setData(commandIndex * sizeof(DrawElementsIndirectCommand) + sizeof(GLuint), sizeof(GLuint), &instanceCount);
    }
  }
}