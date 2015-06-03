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

      void createBuffer(GLenum textureTarget, GLuint bufferBlockSize, GLuint size, GLenum usage, GLenum internalFormat, const GLvoid *data = nullptr);

      void resizeBuffer(GLuint size);
      bool isEnoughSpace(GLuint size) const;

      void setData(GLuint offset, GLuint size, const GLvoid *data);
      void getData(GLuint offset, GLuint size, GLvoid *data) const;

      void clearBuffer(GLenum format, GLenum type, const GLvoid *data) const;

      void bindTexture(GLenum slot) const;
      void unbindTexture(GLenum slot) const;

      void bindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format) const;
      void unbindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format) const;

      void bindBuffer(GLenum target) const;
      void unbindBuffer(GLenum target) const;

      void bindBuffer(GLenum target, GLuint bufferBindingPoint) const;
      void unbindBuffer(GLenum target, GLuint bufferBindingPoint) const;

    private:

      TBO(const TBO&);
      TBO& operator=(const TBO&);

      GLuint m_bufferIndex;
      GLuint m_textureIndex;

      GLuint m_bufferBlockSize;
      GLuint m_currentBufferSize;

      GLenum m_usage;
      GLenum m_textureTarget;
      GLenum m_internalFormat;
    };
  }
}
#endif
