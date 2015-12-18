#ifndef GEOMETRYRENDERER_H_
#define GEOMETRYRENDERER_H_

#include <list>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Renderpass/FrustumCullingRenderpass.h"
#include "Renderer/Renderpass/GeometryRenderpass.h"
#include "Renderer/Renderpass/NoLightingGeometryRenderpass.h"
#include "Renderer/Renderpass/ReflectiveShadowMapGeometryRenderpass.h"
#include "Renderer/Renderpass/ShadowMapGeometryRenderpass.h"
#include "Renderer/Renderpass/ParaboloidShadowMapGeometryRenderpass.h"
#include "Renderer/Renderpass/IndirectBackprojectionRenderpass.h"

#include "Renderer/Renderdata/IRenderContainer.h"

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

    enum Renderpass
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

      void frustumCulling(int cameraIndex, Renderpass pass);

      void generateShadowMap(unsigned int lightIndex);

      void generateParaboloidShadowMap(unsigned int lightIndex);

      void generateReflectiveShadowMap(unsigned int lightIndex);

      void generateIndirectBackprojectionMap(unsigned int lightIndex);

      void rasterizeGeometry();

      void rasterizeNoLightingGeometry();

      void addRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry);

      void removeRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry);

      unsigned int getGlobalOccluderNumber() const;

    private:

      GeometryRenderer(const GeometryRenderer&);
      GeometryRenderer& operator=(const GeometryRenderer&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<sh::ShaderContainer> m_container;

      unsigned int m_globalOccluderNumber;

      unsigned int m_globalVertexNumber;
      unsigned int m_globalTriangleNumber;

      std::vector<util::ResourceHandle> m_updateMeshHandles;
      std::vector<util::ResourceHandle> m_updateMaterialHandles;

      FrustumCullingRenderpass m_frustumCullingRenderpass;
      GeometryRenderpass m_geometryRenderpass;
      NoLightingGeometryRenderpass m_noLightingRenderpass;
      ReflectiveShadowMapGeometryRenderpass m_reflectiveShadowMapRenderpass;
      ShadowMapGeometryRenderpass m_shadowMapRenderpass;
      ParaboloidShadowMapGeometryRenderpass m_paraboloidShadowMappingpass;
      IndirectBackprojectionRenderpass m_indirectBackprojetionRenderpass;

      std::vector<util::SharedPointer<IRenderContainer>> m_renderContainer;
    };
  }
}

#endif
