#ifndef TBO_H_
#define TBO_H_

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

class GRAPHICAPI TBO
{
public:

  TBO(){};
  TBO( GLuint size, GLenum usage, GLenum format, unsigned char *data = nullptr );
  ~TBO();

  void createBuffer(GLuint size, GLenum usage, GLenum format, unsigned char *data = nullptr);
  void setData(unsigned char *data, GLuint offset, GLuint size);

  void setBindingPoint(GLenum slot);

  void bindBuffer() const;
  void unBindBuffer() const;

private:

  TBO(const TBO&){}
	TBO& operator=(const TBO&){ return *this; }

  GLuint m_bufferIndex;
  GLuint m_textureIndex;
  GLuint m_offset;
  GLuint m_size;

  GLenum m_textureSlot;
};

#endif