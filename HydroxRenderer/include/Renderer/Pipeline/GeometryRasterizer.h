#ifndef GEOMETRYRASTERIZER_H_
#define GEOMETRYRASTERIZER_H_

#include <list>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>

#include <SceneGraph/TreeNodes/GeoNode.h>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/RenderTree/GroupNode.h"
#include "Renderer/RenderTree/RenderNode.h"

namespace he
{
	namespace renderer
	{
    class GeometryRasterizer
    {
    public:

      GeometryRasterizer();
      ~GeometryRasterizer();

      void initialize(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle, unsigned int nodeCacheBlockSize = 8);

      void rasterizeGeometry();

      void addRenderComponent(sg::GeoNode *node);
      void removeRenderComponent(sg::GeoNode *node);

    private:

      void registerRenderComponentSlots(util::EventManager *eventManager);

      void frustumCulling();

      std::list<RenderNode*> m_renderNodes;
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
