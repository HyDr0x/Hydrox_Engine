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
      bool isEnoughSpace(GLuint size) const;

      void setData(GLuint offset, GLuint size, const void *data) const;
      void getData(GLuint offset, GLuint size, void *data) const;

      void bindBuffer(GLenum target) const;
      void unbindBuffer(GLenum target) const;

      void bindBuffer(GLenum target, GLuint bufferBindingPoint) const;
      void unbindBuffer(GLenum target, GLuint bufferBindingPoint) const;

      void bindVertexbuffer(GLuint bufferBindingPoint, GLuint offset, GLuint stride) const;
      void unbindVertexBuffer(GLuint bufferBindingPoint) const;

    private:

      GPUBuffer(const GPUBuffer&);
	    GPUBuffer& operator=(const GPUBuffer&);

      GLuint m_bufferIndex;
      
      GLuint m_bufferBlockSize;
      GLuint m_currentBufferSize;

      GLenum m_usage;
      GLenum m_target;
    };
  }
}

#endif
