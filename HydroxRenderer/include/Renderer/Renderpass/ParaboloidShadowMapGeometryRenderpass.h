#ifndef PARABOLOIDSHADOWMAPGEOMETRYRENDERPASS_H_
#define PARABOLOIDSHADOWMAPGEOMETRYRENDERPASS_H_

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

    class ParaboloidShadowMapGeometryRenderpass : public ARenderpass
    {
    public:

      ParaboloidShadowMapGeometryRenderpass();
      virtual ~ParaboloidShadowMapGeometryRenderpass();

      void setLightIndex(int lightIndex);

      virtual void drawRenderpass() const override;

    private:

      int m_lightIndex;
    };
  }
}

#endif
