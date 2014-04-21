#include "Renderer/Buffer/GPUImmutableBuffer.h"

#include <assert.h>

namespace he
{
	namespace renderer
	{
    GPUImmutableBuffer::GPUImmutableBuffer()
    {
      glGenBuffers(1, &m_immutableBufferIndex);
    }

    GPUImmutableBuffer::~GPUImmutableBuffer()
    {
      glDeleteBuffers(1, &m_immutableBufferIndex);
    }

    void GPUImmutableBuffer::createBuffer(GLenum target, GLuint bufferBlockSize, GLuint size, GLenum flags, const void *data)
    {
      m_target = target;
      m_flags = flags;
      m_bufferBlockSize = bufferBlockSize;

      resizeBuffer(size, data);
    }

    void GPUImmutableBuffer::resizeBuffer(GLuint size, const void *data)
    {
      m_currentBufferSize = (size / m_bufferBlockSize + 1) * m_bufferBlockSize;

      glBindBuffer(m_target, m_immutableBufferIndex);
      glBufferStorage(m_target, m_currentBufferSize, data, m_flags);
      m_bufferPointer = glMapBufferRange(m_target, 0, m_currentBufferSize, m_flags);
      glBindBuffer(m_target, 0);
    }

    bool GPUImmutableBuffer::isEnoughSpace(GLuint size)
    {
      return size < m_currentBufferSize;
    }

    void GPUImmutableBuffer::setMemoryFence()
    {
      glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
      m_memoryFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    void GPUImmutableBuffer::syncWithFence()
    {
      glClientWaitSync(m_memoryFence, GL_SYNC_FLUSH_COMMANDS_BIT, 100000);
    }
    
    void GPUImmutableBuffer::setData(GLuint offset, GLuint size, const void *data)
    {
      assert(offset + size <= m_currentBufferSize);

      memcpy(((GLubyte*)m_bufferPointer) + offset, data, size);
    }

    void GPUImmutableBuffer::getData(GLuint offset, GLuint size, GLvoid *data)
    {
      memcpy(data, ((GLubyte*)m_bufferPointer) + offset, size);
    }

    void GPUImmutableBuffer::bindBuffer(GLenum target)
    {
      glBindBuffer(target, m_immutableBufferIndex);
    }

    void GPUImmutableBuffer::unbindBuffer(GLenum target)
    {
      glBindBuffer(target, 0);
    }

    void GPUImmutableBuffer::bindBuffer(GLenum target, GLuint bufferBindingPoint)
    {
      glBindBufferBase(target, bufferBindingPoint, m_immutableBufferIndex);
    }

    void GPUImmutableBuffer::unbindBuffer(GLenum target, GLuint bufferBindingPoint)
    {
      glBindBufferBase(target, bufferBindingPoint, 0);
    }

    void GPUImmutableBuffer::bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride)
    {
      glBindVertexBuffer(bufferBindingPoint, m_immutableBufferIndex, offset, stride);
    }

    void GPUImmutableBuffer::unbindVertexBuffer(GLuint bufferBindingPoint)
    {
      glBindVertexBuffer(bufferBindingPoint, 0, 0, 0);
    }
	}
}
