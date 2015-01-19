#include "Renderer/Buffer/TBO.h"

#include <cassert>
#include <vector>
#include <cmath>

namespace he
{
  namespace renderer
  {
    TBO::TBO() : m_textureSlot(0)
    {
      glGenBuffers(1, &m_bufferIndex);
      glGenTextures(1, &m_textureIndex);
    }

    TBO::~TBO()
    {
      glDeleteBuffers(1, &m_bufferIndex);
      glDeleteTextures(1, &m_textureIndex);
    }

    void TBO::createBuffer(GLenum textureTarget, GLuint bufferBlockSize, GLuint size, GLenum usage, GLenum internalFormat, const GLvoid *data)
    {
      m_usage = usage;
      m_textureTarget = textureTarget;
      m_internalFormat = internalFormat;
      m_bufferBlockSize = bufferBlockSize;
      m_currentBufferSize = 0;

      setData(0, size, data);
    }

    void TBO::resizeBuffer(GLuint size)
    {
      m_currentBufferSize = ceil(size / float(m_bufferBlockSize)) * m_bufferBlockSize;

      glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
      glBufferData(GL_TEXTURE_BUFFER, m_currentBufferSize, nullptr, m_usage);
      glBindBuffer(GL_TEXTURE_BUFFER, 0);

      glBindTexture(m_textureTarget, m_textureIndex);
      glTexBuffer(m_textureTarget, m_internalFormat, m_bufferIndex);//link the created buffer object with the texture
      glBindTexture(m_textureTarget, 0);
    }

    bool TBO::isEnoughSpace(GLuint size) const
    {
      return size < m_currentBufferSize;
    }

    void TBO::setData(GLuint offset, GLuint size, const GLvoid *data)
    {
      if(offset + size > m_currentBufferSize)//is the old buffer big enough for the new data?
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

      glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
      glBufferSubData(GL_TEXTURE_BUFFER, offset, size, data);
      glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    void TBO::getData(GLuint offset, GLuint size, GLvoid *data) const
    {
      glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
      glGetBufferSubData(GL_TEXTURE_BUFFER, offset, size, data);
      glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    void TBO::clearBuffer(GLenum format, GLenum type, const GLvoid *data) const
    {
      glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
      glClearBufferData(GL_TEXTURE_BUFFER, m_internalFormat, format, type, data);
      glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    void TBO::bindTextureBuffer(GLenum slot)
    {
      m_textureSlot = slot;
      glActiveTexture(GL_TEXTURE0 + m_textureSlot);
      glBindTexture(m_textureTarget, m_textureIndex);
    }

    void TBO::unBindTextureBuffer() const
    {
      glActiveTexture(GL_TEXTURE0 + m_textureSlot);
      glBindTexture(m_textureTarget, 0);
    }

    void TBO::bindBuffer() const
    {
      glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
    }

    void TBO::unbindBuffer() const
    {
      glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    void TBO::bindBuffer(GLuint bufferBindingPoint) const
    {
      glBindBufferBase(GL_TEXTURE_BUFFER, bufferBindingPoint, m_bufferIndex);
    }

    void TBO::unbindBuffer(GLuint bufferBindingPoint) const
    {
      glBindBufferBase(GL_TEXTURE_BUFFER, bufferBindingPoint, 0);
    }
  }
}
