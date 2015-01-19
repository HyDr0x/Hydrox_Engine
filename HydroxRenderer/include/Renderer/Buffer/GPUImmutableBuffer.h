#ifndef GPUIMMUTABLEBUFFER_H_
#define GPUIMMUTABLEBUFFER_H_

#include <list>
#include <utility>

#include <GL/glew.h>

namespace he
{
  namespace renderer
  {
    class GPUImmutableBuffer
    {
    public:

      GPUImmutableBuffer();
      ~GPUImmutableBuffer();

      void createBuffer(GLenum target, GLuint bufferBlockSize, GLuint size, GLenum flags, const GLvoid *data = nullptr);

      void resizeBuffer(GLuint size);
      bool isEnoughSpace(GLuint size) const;

      void setMemoryFence();
      void syncWithFence() const;

      void setData(GLuint offset, GLuint size, const GLvoid *data);
      void getData(GLuint offset, GLuint size, GLvoid *data) const;

      void clearBuffer(GLenum format, GLenum internalFormat, GLenum type, const void *data) const;

      void bindBuffer(GLenum target) const;
      void unbindBuffer(GLenum target) const;

      void bindBuffer(GLenum target, GLuint bufferBindingPoint) const;
      void unbindBuffer(GLenum target, GLuint bufferBindingPoint) const;

      void bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride) const;
      void unbindVertexBuffer(GLuint bufferBindingPoint) const;

    private:

      GPUImmutableBuffer(const GPUImmutableBuffer&);
      GPUImmutableBuffer& operator=(const GPUImmutableBuffer&);


      GLvoid *m_bufferPointer;
      GLsync m_memoryFence;
      GLuint m_immutableBufferIndex;
      
      GLuint m_bufferBlockSize;
      GLuint m_currentBufferSize;

      GLenum m_flags;
      GLenum m_target;
    };
  }
}

#endif
