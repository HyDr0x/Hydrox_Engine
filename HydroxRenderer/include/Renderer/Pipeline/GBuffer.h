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

  namespace db
  {
    class Texture2D;
  }

  namespace renderer
  {
    class GBuffer
    {
    public:

      GBuffer();
      ~GBuffer();

      void initialize(util::SingletonManager *singletonManager);

      void setGBuffer() const;
      void unsetGBuffer() const;

      void setDebugGBuffer() const;
      void unsetDebugGBuffer() const;

      void setClearColor(util::vec4f clearColor);

      util::SharedPointer<db::Texture2D> getDepthTexture() const;
      util::SharedPointer<db::Texture2D> getColorTexture() const;
      util::SharedPointer<db::Texture2D> getNormalTexture() const;
      util::SharedPointer<db::Texture2D> getMaterialTexture() const;

      void clear() const;

    private:

      GBuffer(const GBuffer&);
      GBuffer& operator=(const GBuffer&);

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<db::Texture2D> m_depthTexture;
      util::SharedPointer<db::Texture2D> m_colorTexture;
      util::SharedPointer<db::Texture2D> m_normalTexture;
      util::SharedPointer<db::Texture2D> m_materialTexture;

      Renderquad m_fullscreenRenderQuad;
      Renderquad m_debugRenderquad;

      util::vec4f m_clearColor;
    };
  }
}

#endif
