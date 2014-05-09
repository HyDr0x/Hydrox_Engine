#ifndef UBO_H_
#define UBO_H_

#include <vector>

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

namespace he
{
	namespace renderer
	{
    class Shader;

    class GRAPHICAPI UBO
    {
    public:

      UBO();
	    ~UBO();

      void createBuffer(Shader *shader, const GLuint bufferSize, const GLuint uniformCount, GLenum usage, const char **uniformNames);//shared layout
      void createBuffer(const GLuint bufferSize, GLenum usage);//std140 layout

	    template<class T> void setData(const GLuint index, T *data, const GLuint count = 1)//shared layout
	    {
		    m_bufferDataOffset = m_uniformOffsets[index];
		    for( int i = 0; i != count ; i++ )
		    {
			    assert( m_bufferDataOffset <= m_bufferSize );
			    *reinterpret_cast<T*>( m_bufferData + m_bufferDataOffset ) = data[i];
			    m_bufferDataOffset += m_arrayStrides[index];
		    }
	    }

	    template<class T> void setMatrixData(const GLuint index, T *data, const GLuint rows, const GLuint columns)//shared layout
	    {
        m_bufferDataOffset = m_uniformOffsets[index];
        for(int i = 0; i != rows; i++ )
		    {
			    for( int j = 0; j != columns; j++ )
			    {
				    assert( m_bufferDataOffset <= m_bufferSize );
				    *reinterpret_cast<T*>( m_bufferData + m_bufferDataOffset ) = data[i * columns + j];
				    m_bufferDataOffset += sizeof( data[0] );
			    }
		    }
	    }

      void setData(GLuint offset, GLuint size, const void *data) const;//standard layout
      void getData(GLuint offset, GLuint size, void *data) const;

	    void uploadData() const;

	    void bindBuffer(GLuint bufferBindingPoint) const;
      void unBindBuffer(GLuint bufferBindingPoint) const;

    private:

	    UBO(const UBO&);
	    UBO& operator=(const UBO&);

      std::vector<GLuint> m_uniformIndices; 
      std::vector<GLint> m_uniformOffsets;
      std::vector<GLint> m_arrayStrides; 
      std::vector<GLint> m_matrixStrides;

      GLubyte *m_bufferData;
	    GLuint m_bufferIndex;
	    GLuint m_bufferSize;
	    GLuint m_bufferDataOffset;
	
      GLuint m_bufferBindingPoint;

	    GLuint m_uniformCount;
    };
	}
}
#endif
