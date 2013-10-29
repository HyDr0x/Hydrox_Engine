#ifndef UBO_H_
#define UBO_H_

#include <vector>

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

class Shader;

class GRAPHICAPI UBO
{
public:

  UBO();
	UBO(Shader *shader, const GLuint bufferSize, const GLuint uniformCount, GLenum usage, const char **uniformNames);//shared layout constructor
  UBO(const GLuint bufferSize, const GLuint uniformCount, GLenum usage);//std140 layout constructor
	~UBO();

  void createBuffer(Shader *shader, const GLuint bufferSize, const GLuint uniformCount, GLenum usage, const char **uniformNames);//shared layout
  void createBuffer(const GLuint bufferSize, const GLuint uniformCount, GLenum usage);//std140 layout

	template<class T> void setData( const GLuint index, T *data, const GLuint count = 1 )//shared layout
	{
		bufferDataOffset = uniformOffsets[index];
		for( int i = 0; i != count ; i++ )
		{
			assert( bufferDataOffset <= bufferSize );
			*reinterpret_cast<T*>( bufferData + bufferDataOffset ) = data[i];
			bufferDataOffset += arrayStrides[index];
		}
	}

	template<class T> void setMatrixData( const GLuint index, T *data, const GLuint rows, const GLuint columns )//shared layout
	{
    bufferDataOffset = uniformOffsets[index];
    for(int i = 0; i != rows; i++ )
		{
			for( int j = 0; j != columns; j++ )
			{
				assert( bufferDataOffset <= bufferSize );
				*reinterpret_cast<T*>( bufferData + bufferDataOffset ) = data[i * columns + j];
				bufferDataOffset += sizeof( data[0] );
			}
		}
	}

  void setData(unsigned char *data, unsigned int offset, const GLuint size );//standart layout

	void uploadData() const;

  void setBindingPoint(GLuint bindingPoint);

	void bindBuffer() const;
  void unBindBuffer() const;

private:

	UBO(const UBO&){}
	UBO& operator=(const UBO&){ return *this; }

  std::vector<GLuint> m_uniformIndices; 
  std::vector<GLint> m_uniformOffsets;
  std::vector<GLint> m_arrayStrides; 
  std::vector<GLint> m_matrixStrides;

  unsigned char *m_bufferData;
	GLuint m_bufferIndex;
	GLuint m_bufferSize;
	GLuint m_bufferDataOffset;
	
  GLuint m_bufferBindingPoint;

	GLuint m_uniformCount;
};

#endif