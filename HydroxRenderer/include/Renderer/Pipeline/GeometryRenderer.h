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

namespace he
{
  namespace xBar
  {
    class IGeometryContainer;
  }

  namespace renderer
  {
    class GroupNode;
    class IRenderNode;
    class RenderShaderContainer;

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

      void frustumCulling(int shadowMapIndex, RenderPass pass);

      void generateShadowMap(int shadowMapIndex, RenderPass pass);

      void rasterizeIndexGeometry();

      void rasterizeGeometry();

      void addRenderComponent(const xBar::IGeometryContainer& geometry);

      void removeRenderComponent(const xBar::IGeometryContainer& geometry);

      unsigned int getGlobalCacheNumber() const;

    private:

      GeometryRenderer(const GeometryRenderer&);
      GeometryRenderer& operator=(const GeometryRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      RenderShaderContainer *m_container;

      unsigned int m_globalCacheNumber;
      
      GroupNode *m_renderRootNode;
      GroupNode *m_renderIndexRootNode;
      GroupNode *m_renderShadowRootNode;
      GroupNode *m_renderReflectiveShadowRootNode;

      util::SingletonManager *m_singletonManager;
    };
  }
}

#endif
