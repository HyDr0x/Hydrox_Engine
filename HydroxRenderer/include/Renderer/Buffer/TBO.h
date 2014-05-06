#ifndef TBO_H_
#define TBO_H_

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

namespace he
{
	namespace renderer
	{
    class GRAPHICAPI TBO
    {
    public:

      TBO();
      ~TBO();

      void createBuffer(GLuint size, GLenum usage, GLenum format, void *data = nullptr);

      void setData(GLuint offset, GLuint size, void *data);
      void getData(GLuint offset, GLuint size, void *data);

      void bindBuffer(GLenum slot);
      void unBindBuffer() const;

    private:

      TBO(const TBO&){}
	    TBO& operator=(const TBO&){ return *this; }

      GLuint m_bufferIndex;
      GLuint m_textureIndex;

      GLenum m_textureSlot;
    };
	}
}
#endif
