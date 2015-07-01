#ifndef INDIRECTLIGHTGEOMETRYRENDERPASS_H_
#define INDIRECTLIGHTGEOMETRYRENDERPASS_H_

#include <list>

#include <Utilities/Math/Math.hpp>

#include <XBar/StaticGeometryContainer.h>

#include <DataBase/ResourceManager.hpp>

#include <Shader/ShaderContainer.h>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/Renderpass/ARenderpass.h"

namespace he
{
  namespace renderer
  {
    class RenderOptions;

    class IndirectLightGeometryRenderpass : public ARenderpass
    {
    public:

      IndirectLightGeometryRenderpass();
      virtual ~IndirectLightGeometryRenderpass();

      virtual void initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Renderpass renderPass,
        util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap);

      void setProxyLightTextureResolution(GLuint proxyLightTextureResolution);
      void setIndirectShadowMap(util::SharedPointer<db::Texture2D> indirectShadowMap);

      virtual void drawRenderpass() const override;

    private:

      GLuint m_proxyLightTextureResolution;

      util::SharedPointer<db::Texture2D> m_normalMap;
      util::SharedPointer<db::Texture2D> m_materialMap;
      util::SharedPointer<db::Texture2D> m_indirectShadowMap;
    };
  }
}

#endif
