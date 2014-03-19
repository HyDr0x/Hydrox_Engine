#include "Renderer/Buffer/GPUBuffer.h"

#include <assert.h>

namespace he
{
	namespace renderer
	{
    GPUBuffer::GPUBuffer()
    {
      glGenBuffers(1, &m_bufferIndex);
    }

    GPUBuffer::~GPUBuffer()
    {
      glDeleteBuffers(1, &m_bufferIndex);
    }

    void GPUBuffer::createBuffer(GLenum target, GLuint bufferBlockSize, GLuint size, GLenum usage, const void *data)
    {
      m_target = target;
      m_usage = usage;
      m_bufferBlockSize = bufferBlockSize;

      resizeBuffer(size, data);
    }

    void GPUBuffer::resizeBuffer(GLuint size, const void *data)
    {
      m_currentBufferSize = (size / m_bufferBlockSize + 1) * m_bufferBlockSize;

      glBindBuffer(m_target, m_bufferIndex);
      glBufferData(m_target, m_currentBufferSize, data, m_usage);
      glBindBuffer(m_target, 0);
    }

    bool GPUBuffer::isEnoughSpace(GLuint size)
    {
      return size < m_currentBufferSize;
    }

    void GPUBuffer::setData(GLuint offset, GLuint size, const void *data)
    {
      assert(offset + size <= m_currentBufferSize);

      glBindBuffer(m_target, m_bufferIndex);
      glBufferSubData(m_target, offset, size, data);
      glBindBuffer(m_target, 0);
    }

    void GPUBuffer::getData(GLuint offset, GLuint size, GLvoid *data)
    {
      glBindBuffer(m_target, m_bufferIndex);
      glGetBufferSubData(m_target, offset, size, data);
      glBindBuffer(m_target, 0);
    }

    void GPUBuffer::bindBuffer(GLenum target)
    {
      glBindBuffer(target, m_bufferIndex);
    }

    void GPUBuffer::unbindBuffer(GLenum target)
    {
      glBindBuffer(target, 0);
    }

    void GPUBuffer::bindBuffer(GLenum target, GLuint bufferBindingPoint)
    {
      glBindBufferBase(target, bufferBindingPoint, m_bufferIndex);
    }

    void GPUBuffer::unbindBuffer(GLenum target, GLuint bufferBindingPoint)
    {
      glBindBufferBase(target, bufferBindingPoint, 0);
    }

    void GPUBuffer::bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride)
    {
      glBindVertexBuffer(bufferBindingPoint, m_bufferIndex, offset, stride);
    }

    void GPUBuffer::unbindVertexBuffer(GLuint bufferBindingPoint)
    {
      glBindVertexBuffer(bufferBindingPoint, 0, 0, 0);
    }
	}
}
