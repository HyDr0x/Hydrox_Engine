#ifndef SSBO_H_
#define SSBO_H_

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

namespace he
{
	namespace renderer
	{
    class GRAPHICAPI SSBO
    {
    public:

      SSBO();
      ~SSBO();

      void createBuffer(GLuint size, GLenum usage, void *data = nullptr);

      void setData(void *data, GLuint offset, GLuint size);
      void getData(void *data, GLuint offset, GLuint size);

      void setBindingPoint(GLuint bindingPoint);

      void bindBuffer() const;
      void unBindBuffer() const;

    private:

	    SSBO(const SSBO&){}
	    SSBO& operator=(const SSBO&){ return *this; }

      GLuint m_bufferIndex;
      GLuint m_bufferBindingPoint;
    };
	}
}

#endif
