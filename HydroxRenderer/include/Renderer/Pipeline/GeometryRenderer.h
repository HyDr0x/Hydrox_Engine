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
#include "Renderer/Traverser/RenderIndexGeometryTraverser.h"
#include "Renderer/Traverser/RenderShadowGeometryTraverser.h"
#include "Renderer/Traverser/FrustumCullingTraverser.h"

namespace he
{
  namespace xBar
  {
    class IGeometryContainer;
  }

  namespace db
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

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer();

      void frustumCulling(int cameraIndex, RenderPass pass);

      void generateShadowMap(int cameraIndex, RenderPass pass);

      void rasterizeIndexGeometry();

      void rasterizeGeometry();

      void addRenderComponent(const xBar::IGeometryContainer& geometry);

      void removeRenderComponent(const xBar::IGeometryContainer& geometry);

      unsigned int getGlobalCacheNumber() const;

    private:

      GeometryRenderer(const GeometryRenderer&);
      GeometryRenderer& operator=(const GeometryRenderer&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<db::ShaderContainer> m_container;

      unsigned int m_globalCacheNumber;

      FrustumCullingTraverser m_frustumCullingTraverser;
      RenderGeometryTraverser m_renderGeometryTraverser;
      RenderIndexGeometryTraverser m_renderIndexGeometryTraverser;
      RenderShadowGeometryTraverser m_renderShadowGeometryTraverser;

      GroupNode *m_renderRootNode;
      GroupNode *m_renderIndexRootNode;
      GroupNode *m_renderShadowRootNode;
      GroupNode *m_renderReflectiveShadowRootNode;
    };
  }
}

#endif
