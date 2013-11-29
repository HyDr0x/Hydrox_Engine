#include "Hydrox/Graphics/SSBO.h"

namespace he
{
  SSBO::SSBO( GLuint size, GLenum usage, unsigned char *data) : m_size(size),
                                                                m_offset(0),
                                                                m_bufferBindingPoint(0)
  {
    glGenBuffers(1, &m_bufferIndex);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferIndex);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_size, data, usage);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  SSBO::~SSBO()
  {
    glDeleteBuffers(1, &m_bufferIndex);
  }

  void SSBO::createBuffer(GLuint size, GLenum usage, unsigned char *data)
  {
    m_size = size;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferIndex);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_size, data, usage);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void SSBO::setData(unsigned char *data, GLuint offset, GLuint size)
  {
    m_offset = offset;
    m_size = size;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferIndex);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, m_offset, m_size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void SSBO::setBindingPoint(GLuint bindingPoint)
  {
    m_bufferBindingPoint = bindingPoint;
  }

  void SSBO::bindBuffer() const
  {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_bufferBindingPoint, m_bufferIndex);
  }

  void SSBO::unBindBuffer() const
  {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_bufferBindingPoint, 0);
  }
}
