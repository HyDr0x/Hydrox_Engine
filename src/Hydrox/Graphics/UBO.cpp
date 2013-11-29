#include "Hydrox/Graphics/UBO.h"

#include "Hydrox/Graphics/Shader.h"

namespace he
{
  UBO::UBO() : m_bufferData(NULL)
  {
  }

  UBO::UBO(Shader *shader, const GLuint bufferSize, const GLuint uniformCount, GLenum usage, const char **uniformNames) : m_bufferDataOffset(0),//shared layout constructor
                                                                                                                          m_bufferBindingPoint(0),
                                                                                                                          m_bufferSize(bufferSize),
                                                                                                                          m_uniformCount(uniformCount)
  {
	  glGenBuffers( 1, &m_bufferIndex );
	  glBindBuffer( GL_UNIFORM_BUFFER, m_bufferIndex );
	  glBufferData( GL_UNIFORM_BUFFER, m_bufferSize, NULL, usage );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );

    m_uniformIndices.resize(m_uniformCount);
	  m_uniformOffsets.resize(m_uniformCount);
	  m_arrayStrides.resize(m_uniformCount);
	  m_matrixStrides.resize(m_uniformCount);

    shader->getUniformParameters(uniformNames, m_uniformCount, &m_uniformIndices, &m_uniformOffsets, &m_arrayStrides, &m_matrixStrides);

	  m_bufferData = new unsigned char[bufferSize];
  }

  UBO::UBO(const GLuint bufferSize, const GLuint uniformCount, GLenum usage) : m_bufferDataOffset(0),//std140 layout constructor
                                                                                m_bufferBindingPoint(0),
                                                                                m_bufferSize(bufferSize),
                                                                                m_uniformCount(uniformCount)
  {
    glGenBuffers( 1, &m_bufferIndex );
	  glBindBuffer( GL_UNIFORM_BUFFER, m_bufferIndex );
	  glBufferData( GL_UNIFORM_BUFFER, m_bufferSize, NULL, usage );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );

    m_uniformOffsets.resize(1);
    m_uniformOffsets[0] = 0;

    m_bufferData = new unsigned char[m_bufferSize];
  }

  UBO::~UBO()
  {
    m_uniformIndices.clear();
	  m_uniformOffsets.clear();
	  m_arrayStrides.clear();
	  m_matrixStrides.clear();

    if(m_bufferData != nullptr)
    {
	    delete[] m_bufferData;
    }
  }

  void UBO::createBuffer(Shader *shader, const GLuint bufferSize, const GLuint uniformCount, GLenum usage, const char **uniformNames)//shared layout
  {
    m_uniformCount = uniformCount;
    m_bufferSize = bufferSize;

    glGenBuffers( 1, &m_bufferIndex );
	  glBindBuffer( GL_UNIFORM_BUFFER, m_bufferIndex );
	  glBufferData( GL_UNIFORM_BUFFER, this->m_bufferSize, NULL, usage );

    m_uniformIndices.resize(m_uniformCount);
	  m_uniformOffsets.resize(m_uniformCount);
	  m_arrayStrides.resize(m_uniformCount);
	  m_matrixStrides.resize(m_uniformCount);

    shader->getUniformParameters(uniformNames, m_uniformCount, &m_uniformIndices, &m_uniformOffsets, &m_arrayStrides, &m_matrixStrides);

	  m_bufferData = new unsigned char[bufferSize];
  }

  void UBO::createBuffer(const GLuint bufferSize, const GLuint uniformCount, GLenum usage)//std140 layout
  {
    m_uniformCount = uniformCount;
    m_bufferSize = bufferSize;

    glGenBuffers( 1, &m_bufferIndex );
	  glBindBuffer( GL_UNIFORM_BUFFER, m_bufferIndex );
	  glBufferData( GL_UNIFORM_BUFFER, m_bufferSize, NULL, usage );

    m_bufferData      = new unsigned char[m_bufferSize];
  }

  void UBO::setData(unsigned char *data, unsigned int offset, const GLuint size )
  {
    memcpy( m_bufferData + offset, data, size );
  }

  void UBO::uploadData() const
  {
	  glBindBuffer( GL_UNIFORM_BUFFER, m_bufferIndex );
	  glBufferSubData( GL_UNIFORM_BUFFER, m_uniformOffsets[0], m_bufferSize, m_bufferData );
	  glBindBuffer( GL_UNIFORM_BUFFER, 0);
  }

  void UBO::setBindingPoint(GLuint bindingPoint)
  {
    m_bufferBindingPoint = bindingPoint;
  }

  void UBO::bindBuffer() const
  {
	  glBindBufferBase( GL_UNIFORM_BUFFER, m_bufferBindingPoint, m_bufferIndex );
  }

  void UBO::unBindBuffer() const
  {
    glBindBufferBase( GL_UNIFORM_BUFFER, m_bufferBindingPoint, 0 );
  }
}
