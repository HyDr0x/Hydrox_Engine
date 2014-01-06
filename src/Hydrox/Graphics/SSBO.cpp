#include "Hydrox/Graphics/SSBO.h"

namespace he
{
  SSBO::SSBO() : m_bufferBindingPoint(0)
  {
    glGenBuffers(1, &m_bufferIndex);
  }

  SSBO::~SSBO()
  {
    glDeleteBuffers(1, &m_bufferIndex);
  }

  void SSBO::createBuffer(GLuint size, GLenum usage, void *data)
  {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferIndex);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void SSBO::setData(void *data, GLuint offset, GLuint size)
  {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferIndex);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void SSBO::getData(void *data, GLuint offset, GLuint size)
  {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferIndex);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
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
