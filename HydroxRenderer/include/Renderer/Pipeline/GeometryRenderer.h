#ifndef GEOMETRYRENDERER_H_
#define GEOMETRYRENDERER_H_

#include <list>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/ResourceManager.hpp>
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace xBar
  {
    class StaticGeometryContainer;
    class SkinnedGeometryContainer;
  }

  namespace renderer
  {
    class GroupNode;
    class IRenderNode;

    class GeometryRenderer
    {
    public:

      GeometryRenderer();
      ~GeometryRenderer();

      void initialize(const RenderOptions& options, util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle, util::ResourceHandle shadowMapGenerationShaderHandle, unsigned int nodeCacheBlockSize = 8);

      void generateShadowMap(unsigned int shadowMapIndex);

      void rasterizeGeometry();

      void addRenderComponent(const xBar::StaticGeometryContainer& staticGeometry);
      void addRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry);

      void removeRenderComponent(const xBar::StaticGeometryContainer& staticGeometry);
      void removeRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry);

    private:

      GeometryRenderer(const GeometryRenderer&);
      GeometryRenderer& operator=(const GeometryRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      util::ResourceHandle m_frustumCullingShaderHandle;
      util::ResourceHandle m_shadowMapGenerationShaderHandle;

      RenderOptions m_options;

      GroupNode *m_renderRootNode;

      util::SingletonManager *m_singletonManager;
    };
  }
}

#endif
