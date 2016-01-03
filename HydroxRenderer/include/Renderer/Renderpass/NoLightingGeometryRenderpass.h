#ifndef NOLIGHTINGGEOMETRYRENDERPASS_H_
#define NOLIGHTINGGEOMETRYRENDERPASS_H_

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

    class NoLightingGeometryRenderpass : public ARenderpass
    {
    public:

      NoLightingGeometryRenderpass();
      virtual ~NoLightingGeometryRenderpass();

      virtual void initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Renderpass renderPass, std::vector<util::SharedPointer<SamplerObject>> samplerObjects);

      virtual void drawRenderpass() const override;

    private:

      util::SharedPointer<db::TextureManager> m_textureManager;

      std::vector<util::SharedPointer<SamplerObject>> m_samplerObjects;
    };
  }
}

#endif