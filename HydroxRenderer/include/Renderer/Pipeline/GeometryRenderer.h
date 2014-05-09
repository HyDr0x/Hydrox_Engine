#ifndef GEOMETRYRENDERER_H_
#define GEOMETRYRENDERER_H_

#include <list>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>

#include "Renderer/Resources/ResourceManager.hpp"

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
    struct RenderOptions;

    class GeometryRenderer
    {
    public:

      GeometryRenderer(RenderOptions& options);
      ~GeometryRenderer();

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle, unsigned int nodeCacheBlockSize = 8);

      void rasterizeGeometry();

      void addRenderComponent(xBar::StaticGeometryContainer& staticGeometry);
      void addRenderComponent(xBar::SkinnedGeometryContainer& skinnedGeometry);

      void removeRenderComponent(xBar::StaticGeometryContainer& staticGeometry);
      void removeRenderComponent(xBar::SkinnedGeometryContainer& skinnedGeometry);

    private:

      void registerRenderComponentSlots(util::EventManager *eventManager);

      util::ResourceHandle m_frustumCullingShaderHandle;

      GLuint m_meshVAO;

      RenderOptions& m_options;

      GroupNode *m_renderRootNode;

      util::SingletonManager *m_singletonManager;
    };
	}
}

#endif
