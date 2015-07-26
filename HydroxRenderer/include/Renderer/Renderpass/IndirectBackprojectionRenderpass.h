#ifndef INDIRECTBACKPROJECTIONRENDERPASS_H_
#define INDIRECTBACKPROJECTIONRENDERPASS_H_

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

    class IndirectBackprojectionRenderpass : public ARenderpass
    {
    public:

      IndirectBackprojectionRenderpass();
      virtual ~IndirectBackprojectionRenderpass();

      virtual bool insertGeometry(util::SharedPointer<IRenderContainer> renderContainer) override;

      void setLightIndex(int lightIndex);

      virtual void drawRenderpass() const override;

    private:

      int m_lightIndex;
    };
  }
}

#endif
