#include "Renderer/Buffer/UBO.h"

#include "Renderer/Resources/Shader.h"

namespace he
{
	namespace renderer
	{
	

    UBO::UBO() : m_bufferData(nullptr), 
                 m_bufferDataOffset(0), 
                 m_bufferBindingPoint(0), 
                 m_bufferSize(0), 
                 m_uniformCount(0)
    {
      glGenBuffers(1, &m_bufferIndex);
    }

    UBO::~UBO()
    {
      glDeleteBuffers(1, &m_bufferIndex);

      if(m_bufferData != nullptr)
      {
	      delete[] m_bufferData;
      }
    }

    void UBO::createBuffer(Shader *shader, const GLuint bufferSize, const GLuint uniformCount, GLenum usage, const char **uniformNames)//shared layout
    {
      m_uniformCount = uniformCount;
      m_bufferSize = bufferSize;

      m_uniformIndices.resize(m_uniformCount);
	    m_uniformOffsets.resize(m_uniformCount);
	    m_arrayStrides.resize(m_uniformCount);
	    m_matrixStrides.resize(m_uniformCount);

	    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferIndex);
	    glBufferData(GL_UNIFORM_BUFFER, this->m_bufferSize, NULL, usage);

      shader->getUniformParameters(uniformNames, m_uniformCount, &m_uniformIndices, &m_uniformOffsets, &m_arrayStrides, &m_matrixStrides);

      glBindBuffer(GL_UNIFORM_BUFFER, 0);

	    m_bufferData = new unsigned char[bufferSize];
    }

    void UBO::createBuffer(const GLuint bufferSize, GLenum usage)//std140 layout
    {
      m_bufferSize = bufferSize;
      m_uniformOffsets.resize(1, 0);

	    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferIndex);
	    glBufferData(GL_UNIFORM_BUFFER, m_bufferSize, NULL, usage);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      m_bufferData = new unsigned char[m_bufferSize];
    }

    void UBO::setData(void *data, unsigned int offset, const GLuint size)
    {
      memcpy(m_bufferData + offset, data, size);
    }

    void UBO::getData(void *data, GLuint offset, GLuint size)
    {
      glBindBuffer(GL_UNIFORM_BUFFER, m_bufferIndex);
      glGetBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UBO::uploadData() const
    {
	    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferIndex);
	    glBufferSubData(GL_UNIFORM_BUFFER, m_uniformOffsets[0], m_bufferSize, m_bufferData);
	    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UBO::setBindingPoint(GLuint bindingPoint)
    {
      m_bufferBindingPoint = bindingPoint;
    }

    void UBO::bindBuffer() const
    {
	    glBindBufferBase(GL_UNIFORM_BUFFER, m_bufferBindingPoint, m_bufferIndex);
    }

    void UBO::unBindBuffer() const
    {
      glBindBufferBase(GL_UNIFORM_BUFFER, m_bufferBindingPoint, 0);
    }

	}
}
