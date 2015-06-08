#ifndef INDEXGEOMETRYRENDERPASS_H_
#define INDEXGEOMETRYRENDERPASS_H_

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

    class IndexGeometryRenderpass : public ARenderpass
    {
    public:

      IndexGeometryRenderpass();
      virtual ~IndexGeometryRenderpass();

      void setProxyLightTextureResolution(GLuint proxyLightTextureResolution);

      virtual void drawRenderpass() const override;

    private:

      GLuint m_proxyLightTextureResolution;
    };
  }
}

#endif
