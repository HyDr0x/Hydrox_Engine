#ifndef RENDERMANAGER_H_
#define RENDERMANAGER_H_

#include <list>

#include <GL/glew.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include "Renderer/DLLExport.h"

#include <DataBase/ResourceManager.hpp>
#include "Renderer/Pipeline/GBuffer.h"
#include "Renderer/Pipeline/LightRenderer.h"
#include "Renderer/Pipeline/IndirectLightRenderer.h"
#include "Renderer/Pipeline/IndirectShadowsCreation.h"
#include "Renderer/Pipeline/ParticleRenderer.h"
#include "Renderer/Pipeline/GeometryRenderer.h"
#include "Renderer/Pipeline/BillboardRenderer.h"
#include "Renderer/Pipeline/SkyboxRenderer.h"
#include "Renderer/Pipeline/SpriteRenderer.h"
#include "Renderer/Pipeline/StringRenderer2D.h"
#include "Renderer/Pipeline/FinalCompositing.h"
#include "Renderer/Pipeline/Tonemapper.h"

#include "Renderer/Buffer/UBO.h"

namespace he
{
  namespace xBar
  {
    class SkinnedGeometryContainer;
    class StaticGeometryContainer;
    class BillboardContainer;
    class LightContainer;
    class ParticleEmitterContainer;
  }

  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class RenderOptions;

    class GRAPHICAPI RenderManager : public util::SingletonBehaviour
    {
    public:

      RenderManager();
      ~RenderManager();

      void setClearColor(he::util::vec4f color);

      void resizeRenderWindow(unsigned int width, unsigned int height);

      void setWindingOrder(GLenum windingOrder) const;

      void enableBackfaceCulling(GLenum cullMode);
      void disableBackfaceCulling();

      void setWireframe(bool wireFrame);

      const size_t getMaxSpriteLayer() const;

      void enableSkybox(util::ResourceHandle skyboxTextureHandles[6]);
      void disableSkybox();

      void showDebugTexture(unsigned int debugTexture);

      void showVirtualAreaLights(bool showVirtualAreaLights);

      void showCaches(bool showCaches);

      void initialize(util::SingletonManager *singletonManager);

      void setViewPort(GLuint width, GLuint height, GLfloat zNear, GLfloat zFar);

      void render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::vec3f& cameraPosition);

      void addRenderComponent(const db::Sprite* sprite);
      void addRenderComponent(const StringTexture2D* string);
      void addRenderComponent(const xBar::BillboardContainer& billboard);
      void addRenderComponent(util::SharedPointer<const xBar::StaticGeometryContainer> staticGeometry);
      void addRenderComponent(util::SharedPointer<const xBar::SkinnedGeometryContainer> skinnedGeometry);
      void addRenderComponent(const xBar::LightContainer& light);
      void addRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter);

      void removeRenderComponent(const db::Sprite* sprite);
      void removeRenderComponent(const StringTexture2D* string);
      void removeRenderComponent(const xBar::BillboardContainer& billboard);
      void removeRenderComponent(util::SharedPointer<const xBar::StaticGeometryContainer> staticGeometry);
      void removeRenderComponent(util::SharedPointer<const xBar::SkinnedGeometryContainer> skinnedGeometry);
      void removeRenderComponent(const xBar::LightContainer& light);
      void removeRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter);

    private:

      RenderManager(const RenderManager&);
      RenderManager& operator=(const RenderManager&);

      void insertVirtualAreaLights();
      void removeVirtualAreaLights();
      void updateVirtualAreaLights();

      void insertRenderCaches();
      void removeRenderCaches();
      void updateRenderCaches();

      util::SingletonManager *m_singletonManager;

      UBO m_cameraParameterUBO;

      util::SharedPointer<RenderOptions> m_options;

      GBuffer m_gBuffer;
      LightRenderer m_lightRenderer;
      IndirectLightRenderer m_indirectLightRenderer;
      IndirectShadowsCreation m_indirectShadowsCreation;
      ParticleRenderer m_particleRenderer;
      GeometryRenderer m_geometryRasterizer;
      BillboardRenderer m_billboardRenderer;
      SkyboxRenderer m_skyboxRenderer;
      SpriteRenderer m_spriteRenderer;
      StringRenderer2D m_stringRenderer;
      FinalCompositing m_finalCompositing;
      Tonemapper m_tonemapper;

      GLfloat m_aspectRatio;

      GLenum m_cullMode;
      bool m_cullModeNotBack;

      bool m_skyboxRendering;
      bool m_wireframe;

      unsigned int m_debugTexture;

      bool m_showVirtualAreaLights;
      std::vector<util::SharedPointer<util::Matrix<float, 4>>> m_trfProxyLightMatrices;
      util::ResourceHandle m_debugSphereMaterialHandle;
      util::ResourceHandle m_debugProxyLightMeshHandle;
      
      bool m_showCaches;
      std::vector<util::SharedPointer<util::Matrix<float, 4>>> m_trfCacheMatrices;
      util::ResourceHandle m_debugDiscMaterialHandle;
      util::ResourceHandle m_debugCacheMeshHandle;
    };
  }
}

#endif
