#include "Hydrox/Graphics/TBO.h"

namespace he
{
  TBO::TBO(GLuint size, GLenum usage, GLenum format, unsigned char *data) : m_size(size),
                                                                            m_offset(0),
                                                                            m_textureSlot(0)
  {
    glGenBuffers(1, &m_bufferIndex);
    glGenTextures(1, &m_textureIndex);

    glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
    glBufferData(GL_TEXTURE_BUFFER, m_size, data, usage);

    glBindTexture(GL_TEXTURE_BUFFER, m_textureIndex);
    glTexBuffer(GL_TEXTURE_BUFFER, format, m_bufferIndex);

    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
  }

  TBO::~TBO()
  {
    glDeleteBuffers(1, &m_bufferIndex);
    glDeleteTextures(1, &m_textureIndex);
  }

  void TBO::createBuffer(GLuint size, GLenum usage, GLenum format, unsigned char *data)
  {
    m_size = size;
    glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
    glBufferData(GL_TEXTURE_BUFFER, m_size, data, usage);

    glBindTexture(GL_TEXTURE_BUFFER, m_textureIndex);
    glTexBuffer(GL_TEXTURE_BUFFER, format, m_bufferIndex);

    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
  }

  void TBO::setData(unsigned char *data, GLuint offset, GLuint size)
  {
    m_offset = offset;
    m_size = size;

    glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
    glBufferSubData(GL_TEXTURE_BUFFER, m_offset, m_size, data);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
  }

  void TBO::setBindingPoint(GLuint slot)
  {
    m_textureSlot = slot;
  }

  void TBO::bindBuffer()  const
  {
    glActiveTexture(GL_TEXTURE0 + m_textureSlot);
    glBindTexture(GL_TEXTURE_BUFFER, m_textureIndex);
  }

  void TBO::unBindBuffer() const
  {
    glActiveTexture(GL_TEXTURE0 + m_textureSlot);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
  }
}
