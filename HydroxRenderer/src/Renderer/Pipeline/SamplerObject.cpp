#include "Renderer/Pipeline/SamplerObject.h"


namespace he
{
  namespace renderer
  {
    SamplerObject::SamplerObject(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter)
    {
      glGenSamplers(1, &m_samplerIndex);

      glSamplerParameteri(m_samplerIndex, GL_TEXTURE_WRAP_S, edgeModeS);
      glSamplerParameteri(m_samplerIndex, GL_TEXTURE_WRAP_T, edgeModeT);
      glSamplerParameteri(m_samplerIndex, GL_TEXTURE_MAG_FILTER, magFilter);
      glSamplerParameteri(m_samplerIndex, GL_TEXTURE_MIN_FILTER, minFilter);
    }

    SamplerObject::~SamplerObject()
    {
      glDeleteSamplers(1, &m_samplerIndex);
    }

    void SamplerObject::bindSampler(GLint slot)
    {
      m_slot = slot;
      glActiveTexture(GL_TEXTURE0 + m_slot);
      glBindSampler(m_slot, m_samplerIndex);
    }

    void SamplerObject::unbindSampler() const
    {
      glActiveTexture(GL_TEXTURE0 + m_slot);
      glBindSampler(m_slot, 0);
    }
  }
}
