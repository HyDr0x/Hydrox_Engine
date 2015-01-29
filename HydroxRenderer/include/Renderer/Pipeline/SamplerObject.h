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

      void bindSampler(GLint slot) const;
      void unbindSampler(GLint slot) const;

    private:

      GLuint m_samplerIndex;
    };
  }
}

#endif
