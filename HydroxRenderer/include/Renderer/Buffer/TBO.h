#ifndef TBO_H_
#define TBO_H_

#include <GL/glew.h>

#include <Utilities/Pointer/UniquePointer.h>

#include "Renderer/DLLExport.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI TBO
    {
    public:

      TBO();
      TBO(TBO& other);
      ~TBO();

      const TBO& operator=(TBO other);

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

      void bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride) const;
      void unbindVertexBuffer(GLuint bufferBindingPoint) const;

    private:

      util::UniquePointer<GLuint> m_bufferIndex;
      util::UniquePointer<GLuint> m_textureIndex;

      GLuint m_bufferBlockSize;
      GLuint m_currentBufferSize;

      GLenum m_usage;
      GLenum m_textureTarget;
      GLenum m_internalFormat;
    };
  }
}
#endif
