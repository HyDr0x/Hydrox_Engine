#ifndef GEOMETRYRENDERER_H_
#define GEOMETRYRENDERER_H_

#include <list>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/ResourceManager.hpp>

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

    class GeometryRenderer
    {
    public:

      GeometryRenderer();
      ~GeometryRenderer();

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer();

      void generateShadowMap(int shadowMapIndex);

      void generateReflectiveShadowMap(int shadowMapIndex);

      void rasterizeGeometry();

      void addRenderComponent(const xBar::IGeometryContainer& geometry);

      void removeRenderComponent(const xBar::IGeometryContainer& geometry);

    private:

      GeometryRenderer(const GeometryRenderer&);
      GeometryRenderer& operator=(const GeometryRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      util::ResourceHandle m_frustumCullingShaderHandle;

      util::ResourceHandle m_staticShadowMapGenerationShaderHandle;
      util::ResourceHandle m_skinnedShadowMapGenerationShaderHandle;

      util::ResourceHandle m_staticReflectiveShadowMapGenerationShaderHandle;
      util::ResourceHandle m_staticNormalReflectiveShadowMapGenerationShaderHandle;
      util::ResourceHandle m_skinnedReflectiveShadowMapGenerationShaderHandle;
      util::ResourceHandle m_skinnedNormalReflectiveShadowMapGenerationShaderHandle;

      GroupNode *m_renderRootNode;
      GroupNode *m_renderShadowRootNode;
      GroupNode *m_renderReflectiveShadowRootNode;

      util::SingletonManager *m_singletonManager;
    };
  }
}

#endif
