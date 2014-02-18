#ifndef GPUBUFFER_H_
#define GPUBUFFER_H_

#include <list>
#include <utility>

#include <GL/glew.h>

namespace he
{
  namespace renderer
  {
    class GPUBuffer
    {
    public:

      GPUBuffer();
      ~GPUBuffer();

      void createBuffer(GLenum target, GLuint bufferBlockSize, GLuint size, GLenum usage, const void *data);

      void resizeBuffer(GLuint size, const void *data);
      bool isEnoughSpace(GLuint size);

      void setData(GLuint offset, GLuint size, const void *data);
      void getData(GLuint offset, GLuint size, void *data);

      void bindBuffer();
      void unbindBuffer();

      void bindBuffer(GLuint bufferBindingPoint);
      void unbindBuffer(GLuint bufferBindingPoint);

      void bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride);
      void unbindVertexBuffer(GLuint bufferBindingPoint);

    private:

      GLuint m_bufferIndex;
      
      GLuint m_bufferBlockSize;
      GLuint m_currentBufferSize;

      GLenum m_usage;
      GLenum m_target;
    };
  }
}

#endif
