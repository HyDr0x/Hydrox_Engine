#ifndef SAMPLEROBJECT_H_
#define SAMPLEROBJECT_H_

#include <GL/glew.h>

namespace he
{
  namespace renderer
  {
    class SamplerObject
    {
    public:

      SamplerObject(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter);
      ~SamplerObject();

      void bindSampler(GLint slot);
      void unbindSampler() const;

    private:

      GLuint m_samplerIndex;
      GLint m_slot;
    };
  }
}

#endif
