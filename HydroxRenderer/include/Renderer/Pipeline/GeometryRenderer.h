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

    class GeometryRenderer
    {
    public:

      GeometryRenderer();
      ~GeometryRenderer();

      void initialize(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle, unsigned int nodeCacheBlockSize = 8);

      void rasterizeGeometry();

      void addRenderComponent(xBar::StaticGeometryContainer& staticGeometry);
      void addRenderComponent(xBar::SkinnedGeometryContainer& skinnedGeometry);

      void addInstancedRenderComponent(xBar::StaticGeometryContainer& staticGeometry);
      void addInstancedRenderComponent(xBar::SkinnedGeometryContainer& skinnedGeometry);

      void removeRenderComponent(xBar::StaticGeometryContainer& staticGeometry);
      void removeRenderComponent(xBar::SkinnedGeometryContainer& skinnedGeometry);

    private:

      void registerRenderComponentSlots(util::EventManager *eventManager);

      void frustumCulling();

      std::list<IRenderNode*> m_renderNodes;
      util::ResourceHandle m_frustumCullingShaderHandle;

      GLuint m_meshVAO;

      unsigned int m_maxMaterials;
      unsigned int m_maxGeometry;
      unsigned int m_maxBones;

      GroupNode *m_renderRootNode;

      util::SingletonManager *m_singletonManager;
    };
	}
}

#endif
