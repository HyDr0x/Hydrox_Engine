#ifndef FRUSTUMCULLINGRENDERPASS_H_
#define FRUSTUMCULLINGRENDERPASS_H_

#include <list>

#include <Utilities/Math/Math.hpp>

#include <XBar/StaticGeometryContainer.h>

#include <DataBase/ResourceManager.hpp>

#include <Shader/ShaderContainer.h>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/Renderpass/AComputepass.h"

namespace he
{
  namespace renderer
  {
    class RenderOptions;

    class FrustumCullingRenderpass : public AComputepass
    {
    public:

      FrustumCullingRenderpass();
      virtual ~FrustumCullingRenderpass();

      void setViewIndex(int viewIndex);

      virtual void doComputepass() const override;

    private:

      int m_viewIndex;
    };
  }
}

#endif
