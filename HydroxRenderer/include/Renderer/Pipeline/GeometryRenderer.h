#ifndef GEOMETRYRENDERER_H_
#define GEOMETRYRENDERER_H_

#include <list>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Buffer/GPUBuffer.h"

#include "Renderer/Traverser/RenderGeometryTraverser.h"
#include "Renderer/Traverser/RenderGeometryTraverserDebug.h"
#include "Renderer/Traverser/RenderIndexGeometryTraverser.h"
#include "Renderer/Traverser/RenderShadowGeometryTraverser.h"
#include "Renderer/Traverser/RenderReflectiveShadowGeometryTraverser.h"
#include "Renderer/Traverser/RenderIndirectLightingGeometryTraverser.h"
#include "Renderer/Traverser/FrustumCullingTraverser.h"

namespace he
{
  namespace xBar
  {
    class IGeometryContainer;
  }

  namespace sh
  {
    class ShaderContainer;
  }

  namespace renderer
  {
    class GroupNode;
    class IRenderNode;

    enum RenderPass
    {
      SHADOWPASS,
      REFLECTIVESHADOWPASS,
      VIEWPASS,
      INDEXPASS,
    };

    class GeometryRenderer
    {
    public:

      GeometryRenderer();
      ~GeometryRenderer();

      void initialize(util::SingletonManager *singletonManager, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap);

      void updateBuffer();

      void frustumCulling(int cameraIndex, RenderPass pass);

      void generateShadowMap(int cameraIndex);

      void generateReflectiveShadowMap(int cameraIndex);

      void generateIndirectLightMap();

      void rasterizeIndexGeometry();

      void rasterizeGeometry();

      void rasterizeDebugGeometry();

      void addRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry);

      void removeRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry);

      unsigned int getGlobalCacheNumber() const;

    private:

      GeometryRenderer(const GeometryRenderer&);
      GeometryRenderer& operator=(const GeometryRenderer&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<sh::ShaderContainer> m_container;

      unsigned int m_globalCacheNumber;
      unsigned int m_bufferResolution;

      std::vector<util::ResourceHandle> m_updateMeshHandles;
      std::vector<util::ResourceHandle> m_updateMaterialHandles;

      FrustumCullingTraverser m_frustumCullingTraverser;
      RenderGeometryTraverser m_renderGeometryTraverser;
      RenderGeometryTraverserDebug m_renderGeometryTraverserDebug;
      RenderIndexGeometryTraverser m_renderIndexGeometryTraverser;
      RenderShadowGeometryTraverser m_renderShadowGeometryTraverser;
      RenderReflectiveShadowGeometryTraverser m_renderReflectiveShadowGeometryTraverser;
      RenderIndirectLightingGeometryTraverser m_renderIndirectLightingGeometryTraverser;

      util::SharedPointer<TreeNode> m_renderRootNode;
      util::SharedPointer<TreeNode> m_renderDebugRootNode;
      util::SharedPointer<TreeNode> m_renderIndexRootNode;
      util::SharedPointer<TreeNode> m_renderShadowRootNode;
      util::SharedPointer<TreeNode> m_renderReflectiveShadowRootNode;
      util::SharedPointer<TreeNode> m_renderIndirectLightRootNode;
    };
  }
}

#endif
