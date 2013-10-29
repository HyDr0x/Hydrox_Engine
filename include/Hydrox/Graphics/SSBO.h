#ifndef SSBO_H_
#define SSBO_H_

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

class GRAPHICAPI SSBO
{
public:

  SSBO(){}
  SSBO( GLuint size, GLenum usage, unsigned char *data = nullptr );
  ~SSBO();

  void createBuffer(GLuint size, GLenum usage, unsigned char *data = nullptr);
  void setData(unsigned char *data, GLuint offset, GLuint size);

  void setBindingPoint(GLuint bindingPoint);

  void bindBuffer() const;
  void unBindBuffer() const;

private:

	SSBO(const SSBO&){}
	SSBO& operator=(const SSBO&){ return *this; }

  GLuint m_bufferIndex;
  GLuint m_offset;
  GLuint m_size;

  GLuint m_bufferBindingPoint;
};

#endif