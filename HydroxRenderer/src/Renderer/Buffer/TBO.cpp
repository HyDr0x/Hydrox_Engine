#include "Renderer/Buffer/TBO.h"

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

    void TBO::createBuffer(GLuint size, GLenum usage, GLenum format, void *data)
    {
      glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
      glBufferData(GL_TEXTURE_BUFFER, size, data, usage);

      glBindTexture(GL_TEXTURE_BUFFER, m_textureIndex);
      glTexBuffer(GL_TEXTURE_BUFFER, format, m_bufferIndex);

      glBindTexture(GL_TEXTURE_BUFFER, 0);
      glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    void TBO::setData(void *data, GLuint offset, GLuint size)
    {
      glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
      glBufferSubData(GL_TEXTURE_BUFFER, offset, size, data);
      glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    void TBO::getData(void *data, GLuint offset, GLuint size)
    {
      glBindBuffer(GL_TEXTURE_BUFFER, m_bufferIndex);
      glGetBufferSubData(GL_TEXTURE_BUFFER, offset, size, data);
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
}
