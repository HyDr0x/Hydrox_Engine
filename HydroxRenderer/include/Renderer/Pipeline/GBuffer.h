#ifndef GBUFFER_H_
#define GBUFFER_H_

#include <list>

#include <Utilities/Math/Math.hpp>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>
#include <Utilities/Signals/EventManager.h>

#include <Shader/ShaderContainer.h>

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

      void setColorBuffer() const;
      void unsetColorBuffer() const;

      void calculateLinearDepthBuffer() const;

      void setClearColor(util::vec4f clearColor);

      void useTestGBuffer();

      util::SharedPointer<db::Texture2D> getDepthTexture() const;
      util::SharedPointer<db::Texture2D> getLinearDepthTexture() const;
      util::SharedPointer<db::Texture2D> getColorTexture() const;
      util::SharedPointer<db::Texture2D> getNormalTexture() const;
      util::SharedPointer<db::Texture2D> getVertexNormalTexture() const;
      util::SharedPointer<db::Texture2D> getMaterialTexture() const;

      void clear() const;

    private:

      GBuffer(const GBuffer&);
      GBuffer& operator=(const GBuffer&);

      void createTestGBuffer();

      util::SingletonManager *m_singletonManager;

      std::vector<util::vec4f> m_colorBuffer;
      std::vector<util::vec4f> m_normalBuffer;
      std::vector<util::vec4f> m_vertexNormaluffer;
      std::vector<util::vec4f> m_materialBuffer;
      std::vector<float> m_depthBuffer;
      std::vector<float> m_linearDepthBuffer;

      util::SharedPointer<db::Texture2D> m_depthTexture;
      util::SharedPointer<db::Texture2D> m_linearDepthTexture;
      util::SharedPointer<db::Texture2D> m_colorTexture;
      util::SharedPointer<db::Texture2D> m_normalTexture;
      util::SharedPointer<db::Texture2D> m_vertexNormalTexture;
      util::SharedPointer<db::Texture2D> m_materialTexture;

      sh::ComputeShaderHandle m_linearDepthBufferCreation;

      Renderquad m_fullscreenRenderQuad;
      Renderquad m_fullscreenColorRenderquad;

      util::vec4f m_clearColor;
    };
  }
}

#endif
