#ifndef GEOMETRYRENDERER_H_
#define GEOMETRYRENDERER_H_

#include <list>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>

#include "Renderer/Resources/ResourceManager.hpp"
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

      void initialize(const RenderOptions& options, util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle, unsigned int nodeCacheBlockSize = 8);

      void rasterizeGeometry() const;

      void addRenderComponent(const xBar::StaticGeometryContainer& staticGeometry);
      void addRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry);

      void removeRenderComponent(const xBar::StaticGeometryContainer& staticGeometry);
      void removeRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry);

    private:

      GeometryRenderer(const GeometryRenderer&);
      GeometryRenderer& operator=(const GeometryRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      util::ResourceHandle m_frustumCullingShaderHandle;

      GLuint m_meshVAO;

      RenderOptions m_options;

      GroupNode *m_renderRootNode;

      util::SingletonManager *m_singletonManager;
    };
	}
}

#endif
