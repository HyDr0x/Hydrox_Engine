#include "Renderer/Buffer/UBO.h"

#include "Renderer/Resources/Shader.h"

namespace he
{
	namespace renderer
	{
    UBO::UBO() : m_bufferData(nullptr), 
                 m_bufferDataOffset(0), 
                 m_bufferSize(0), 
                 m_uniformCount(0)
    {
      glGenBuffers(1, &m_bufferIndex);
    }

    UBO::~UBO()
    {
      glDeleteBuffers(1, &m_bufferIndex);

	    delete[] m_bufferData;
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

      delete[] m_bufferData;
      m_bufferData = new GLubyte[bufferSize];
    }

    void UBO::createBuffer(const GLuint bufferSize, GLenum usage)//std140 layout
    {
      m_bufferSize = bufferSize;
      m_uniformOffsets.resize(1, 0);

	    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferIndex);
	    glBufferData(GL_UNIFORM_BUFFER, m_bufferSize, NULL, usage);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      delete[] m_bufferData;
      m_bufferData = new GLubyte[m_bufferSize];
    }

    void UBO::setData(GLuint offset, GLuint size, const void *data) const
    {
      std::copy((GLubyte*)data, (GLubyte*)data + size, m_bufferData + offset);
    }

    void UBO::getData(GLuint offset, GLuint size, void *data) const
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

    void UBO::bindBuffer(GLuint bufferBindingPoint) const
    {
	    glBindBufferBase(GL_UNIFORM_BUFFER, bufferBindingPoint, m_bufferIndex);
    }

    void UBO::unBindBuffer(GLuint bufferBindingPoint) const
    {
      glBindBufferBase(GL_UNIFORM_BUFFER, bufferBindingPoint, 0);
    }
	}
}
