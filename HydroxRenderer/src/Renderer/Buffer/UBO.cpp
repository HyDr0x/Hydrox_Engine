#include "Renderer/Buffer/UBO.h"

#include <Shader/Shader.h>

namespace he
{
  namespace renderer
  {
    UBO::UBO() : m_bufferData(nullptr), 
                 m_bufferDataOffset(0), 
                 m_bufferSize(0), 
                 m_uniformCount(0)
    {
      m_bufferIndex = util::UniquePointer<GLuint>(new GLuint(0));
      glGenBuffers(1, m_bufferIndex.get());
    }

    UBO::UBO(UBO& other)
    {
      m_bufferData = std::move(other.m_bufferData);
      m_bufferIndex = std::move(other.m_bufferIndex);

      m_uniformIndices = other.m_uniformIndices;
      m_uniformOffsets = other.m_uniformOffsets;
      m_arrayStrides = other.m_arrayStrides;
      m_matrixStrides = other.m_matrixStrides;

      m_bufferSize = other.m_bufferSize;
      m_bufferDataOffset = other.m_bufferDataOffset;

      m_uniformCount = other.m_uniformCount;
    }

    UBO::~UBO()
    {
      if(m_bufferIndex)
      {
        glDeleteBuffers(1, m_bufferIndex.get());
      }
    }

    const UBO& UBO::operator=(UBO other)
    {
      m_bufferData = std::move(other.m_bufferData);
      m_bufferIndex = std::move(other.m_bufferIndex);

      std::swap(m_uniformIndices, other.m_uniformIndices);
      std::swap(m_uniformOffsets, other.m_uniformOffsets);
      std::swap(m_arrayStrides, other.m_arrayStrides);
      std::swap(m_matrixStrides, other.m_matrixStrides);

      std::swap(m_bufferSize, other.m_bufferSize);
      std::swap(m_bufferDataOffset, other.m_bufferDataOffset);

      std::swap(m_uniformCount, other.m_uniformCount);

      return *this;
    }

    void UBO::createBuffer(sh::Shader *shader, const GLuint bufferSize, const GLuint uniformCount, GLenum usage, const char **uniformNames)//shared layout
    {
      m_uniformCount = uniformCount;
      m_bufferSize = bufferSize;

      m_uniformIndices.resize(m_uniformCount);
      m_uniformOffsets.resize(m_uniformCount);
      m_arrayStrides.resize(m_uniformCount);
      m_matrixStrides.resize(m_uniformCount);

      glBindBuffer(GL_UNIFORM_BUFFER, *m_bufferIndex);
      glBufferData(GL_UNIFORM_BUFFER, this->m_bufferSize, nullptr, usage);

      shader->getUniformParameters(uniformNames, m_uniformCount, &m_uniformIndices, &m_uniformOffsets, &m_arrayStrides, &m_matrixStrides);

      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      m_bufferData = util::UniquePointer<GLubyte[]>(new GLubyte[m_bufferSize]);
    }

    void UBO::createBuffer(const GLuint bufferSize, GLenum usage)//std140 layout
    {
      m_bufferSize = bufferSize;
      m_uniformOffsets.resize(1, 0);

      glBindBuffer(GL_UNIFORM_BUFFER, *m_bufferIndex);
      glBufferData(GL_UNIFORM_BUFFER, m_bufferSize, nullptr, usage);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      m_bufferData = util::UniquePointer<GLubyte[]>(new GLubyte[m_bufferSize]);
    }

    void UBO::setData(GLuint offset, GLuint size, const void *data)
    {
      std::copy((const GLubyte*)data, (const GLubyte*)data + size, &m_bufferData[0] + offset);
    }

    void UBO::getData(GLuint offset, GLuint size, void *data) const
    {
      glBindBuffer(GL_UNIFORM_BUFFER, *m_bufferIndex);
      glGetBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UBO::uploadData() const
    {
      glBindBuffer(GL_UNIFORM_BUFFER, *m_bufferIndex);
      glBufferSubData(GL_UNIFORM_BUFFER, m_uniformOffsets[0], m_bufferSize, &m_bufferData[0]);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UBO::bindBuffer(GLuint bufferBindingPoint) const
    {
      glBindBufferBase(GL_UNIFORM_BUFFER, bufferBindingPoint, *m_bufferIndex);
    }

    void UBO::unbindBuffer(GLuint bufferBindingPoint) const
    {
      glBindBufferBase(GL_UNIFORM_BUFFER, bufferBindingPoint, 0);
    }
  }
}
