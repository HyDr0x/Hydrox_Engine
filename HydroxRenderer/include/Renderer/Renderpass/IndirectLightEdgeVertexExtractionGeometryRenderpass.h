#ifndef INDIRECTLIGHTEDGEVERTEXEXTRACTIONGEOMETRYRENDERPASS_H_
#define INDIRECTLIGHTEDGEVERTEXEXTRACTIONGEOMETRYRENDERPASS_H_

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

    class IndirectLightEdgeVertexExtractionGeometryRenderpass : public ARenderpass
    {
    public:

      IndirectLightEdgeVertexExtractionGeometryRenderpass();
      virtual ~IndirectLightEdgeVertexExtractionGeometryRenderpass();

      virtual void initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Renderpass renderPass, util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      virtual void drawRenderpass() const override;

    private:

      util::SharedPointer<db::Texture2D> m_gBufferMaterialMap;
    };
  }
}

#endif
