#include "Renderer/Buffer/GPUImmutableBuffer.h"

#include <cassert>
#include <vector>

namespace he
{
  namespace renderer
  {
    GPUImmutableBuffer::GPUImmutableBuffer()
    {
      m_immutableBufferIndex = 0;
      m_memoryFence = 0;
    }

    GPUImmutableBuffer::~GPUImmutableBuffer()
    {
      glDeleteBuffers(1, &m_immutableBufferIndex);
    }

    void GPUImmutableBuffer::createBuffer(GLenum target, GLuint bufferBlockSize, GLuint size, GLenum flags, const GLvoid *data)
    {
      m_target = target;
      m_flags = flags;
      m_bufferBlockSize = bufferBlockSize;
      m_currentBufferSize = 0;

      setData(0, size, data);
    }

    void GPUImmutableBuffer::resizeBuffer(GLuint size)
    {
      m_currentBufferSize = ceil(size / float(m_bufferBlockSize)) * m_bufferBlockSize;

      glDeleteBuffers(1, &m_immutableBufferIndex);//necessary because of immutable storage
      glGenBuffers(1, &m_immutableBufferIndex);

      glBindBuffer(m_target, m_immutableBufferIndex);
      glBufferStorage(m_target, m_currentBufferSize, nullptr, m_flags);
      m_bufferPointer = glMapBufferRange(m_target, 0, m_currentBufferSize, m_flags);
      glBindBuffer(m_target, 0);
    }

    bool GPUImmutableBuffer::isEnoughSpace(GLuint size) const
    {
      return size < m_currentBufferSize;
    }

    GLuint GPUImmutableBuffer::getBufferSize() const
    {
      return m_currentBufferSize;
    }

    void GPUImmutableBuffer::setMemoryFence()
    {
      glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
      glDeleteSync(m_memoryFence);
      m_memoryFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    void GPUImmutableBuffer::syncWithFence() const
    {
      glClientWaitSync(m_memoryFence, GL_SYNC_FLUSH_COMMANDS_BIT, 100000);
    }
    
    void GPUImmutableBuffer::setData(GLuint offset, GLuint size, const GLvoid *data)
    {
      if(offset + size > m_currentBufferSize)
      {
        if(m_currentBufferSize > 0)
        {
          GLuint oldBufferSize = m_currentBufferSize;
          std::vector<GLubyte> oldData(oldBufferSize);
          getData(0, oldBufferSize, &oldData[0]);//get the old data
          resizeBuffer(offset + size);//resize the buffer
          setData(0, oldBufferSize, &oldData[0]);//fill it with the old data
        }
        else
        {
          resizeBuffer(offset + size);//resize the buffer
        }
      }

      std::copy((GLubyte*)data, (GLubyte*)data + size, (GLubyte*)m_bufferPointer + offset);
    }

    void GPUImmutableBuffer::getData(GLuint offset, GLuint size, GLvoid *data) const
    {
      std::copy((GLubyte*)m_bufferPointer, (GLubyte*)m_bufferPointer + size, (GLubyte*)data + offset);
    }

    void GPUImmutableBuffer::clearBuffer(GLenum format, GLenum internalFormat, GLenum type, const void *data) const
    {
      glBindBuffer(m_target, m_immutableBufferIndex);
      glClearBufferData(m_target, internalFormat, format, type, data);
      glBindBuffer(m_target, 0);
    }

    void GPUImmutableBuffer::bindBuffer(GLenum target) const
    {
      glBindBuffer(target, m_immutableBufferIndex);
    }

    void GPUImmutableBuffer::unbindBuffer(GLenum target) const
    {
      glBindBuffer(target, 0);
    }

    void GPUImmutableBuffer::bindBuffer(GLenum target, GLuint bufferBindingPoint) const
    {
      glBindBufferBase(target, bufferBindingPoint, m_immutableBufferIndex);
    }

    void GPUImmutableBuffer::unbindBuffer(GLenum target, GLuint bufferBindingPoint) const
    {
      glBindBufferBase(target, bufferBindingPoint, 0);
    }

    void GPUImmutableBuffer::bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride) const
    {
      glBindVertexBuffer(bufferBindingPoint, m_immutableBufferIndex, offset, stride);
    }

    void GPUImmutableBuffer::unbindVertexBuffer(GLuint bufferBindingPoint) const
    {
      glBindVertexBuffer(bufferBindingPoint, 0, 0, 0);
    }
  }
}
