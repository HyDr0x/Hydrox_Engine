#ifndef GEOMETRYRASTERIZER_H_
#define GEOMETRYRASTERIZER_H_

#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/RenderTree/GroupNode.h"

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

      void resizeMeshBuffer();
      void resizeMaterialBuffer();

      GLuint m_meshVAO;

      unsigned int m_maxMaterials;
      unsigned int m_maxGeometry;
      unsigned int m_maxBones;

      GroupNode *m_renderRootNode;

      util::ResourceHandle m_cullingShaderHandle;

      util::SingletonManager *m_singletonManager;
    };
	}
}

#endif
