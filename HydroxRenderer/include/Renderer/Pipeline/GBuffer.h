#ifndef GBUFFER_H_
#define GBUFFER_H_

#include <list>

#include <Utilities/Math/Math.hpp>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>
#include <Utilities/Signals/EventManager.h>

#include "Renderer/Pipeline/Renderquad.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class Texture2D;
    struct RenderOptions;

    class GBuffer
    {
    public:

      GBuffer();
      ~GBuffer();

      void initialize(const RenderOptions& renderingOptions, util::SingletonManager *singletonManager, util::ResourceHandle gBufferShaderHandle);

      void setGBuffer() const;
      void unsetGBuffer() const;

      void render() const;

      void clear() const;

    private:

      GBuffer(const GBuffer&);
      GBuffer& operator=(const GBuffer&);

      util::ResourceHandle m_gBufferShaderHandle;

      util::SharedPointer<Texture2D> m_depthTexture;
      util::SharedPointer<Texture2D> m_colorTexture;
      util::SharedPointer<Texture2D> m_normalTexture;

      Renderquad m_fullscreenRenderQuad;
    };
  }
}

#endif
