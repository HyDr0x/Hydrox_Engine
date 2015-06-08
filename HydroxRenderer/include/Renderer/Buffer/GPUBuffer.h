#ifndef GPUBUFFER_H_
#define GPUBUFFER_H_

#include <list>
#include <utility>

#include <GL/glew.h>

#include <Utilities/Pointer/UniquePointer.h>

namespace he
{
  namespace renderer
  {
    class GPUBuffer
    {
    public:

      GPUBuffer();
      GPUBuffer(GPUBuffer& other);
      ~GPUBuffer();

      const GPUBuffer& operator=(GPUBuffer other);

      void createBuffer(GLenum target, GLuint bufferBlockSize, GLuint size, GLenum usage, const GLvoid *data = nullptr);

      void resizeBuffer(GLuint size);
      bool isEnoughSpace(GLuint size) const;

      GLuint getBufferSize() const;

      void setData(GLuint offset, GLuint size, const GLvoid *data);
      void getData(GLuint offset, GLuint size, GLvoid *data) const;

      void clearBuffer(GLenum format, GLenum internalFormat, GLenum type, const GLvoid *data) const;

      void bindBuffer(GLenum target) const;
      void unbindBuffer(GLenum target) const;

      void bindBuffer(GLenum target, GLuint bufferBindingPoint) const;
      void unbindBuffer(GLenum target, GLuint bufferBindingPoint) const;

      void bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride) const;
      void unbindVertexBuffer(GLuint bufferBindingPoint) const;

    private:

      util::UniquePointer<GLuint> m_bufferIndex;
      
      GLuint m_bufferBlockSize;
      GLuint m_currentBufferSize;

      GLenum m_usage;
      GLenum m_target;
    };
  }
}

#endif
