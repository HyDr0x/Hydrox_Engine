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

      void createBuffer(GLenum target, GLuint bufferBlockSize, GLuint size, GLenum flags, const void *data);

      void resizeBuffer(GLuint size, const void *data);
      bool isEnoughSpace(GLuint size);

      void setMemoryFence();
      void syncWithFence();

      void setData(GLuint offset, GLuint size, const void *data);
      void getData(GLuint offset, GLuint size, void *data);

      void bindBuffer(GLenum target);
      void unbindBuffer(GLenum target);

      void bindBuffer(GLenum target, GLuint bufferBindingPoint);
      void unbindBuffer(GLenum target, GLuint bufferBindingPoint);

      void bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride);
      void unbindVertexBuffer(GLuint bufferBindingPoint);

    private:

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
