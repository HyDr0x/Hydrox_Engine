#ifndef RENDERMANAGER_H_
#define RENDERMANAGER_H_

#include <list>

#include <GL/glew.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <XBar/LightContainer.h>
#include <XBar/ParticleContainer.h>

#include "Renderer/DLLExport.h"

#include "Renderer/Resources/ResourceManager.hpp"
#include "Renderer/Pipeline/GBuffer.h"
#include "Renderer/Pipeline/GeometryRenderer.h"
#include "Renderer/Pipeline/BillboardRenderer.h"
#include "Renderer/Pipeline/SkyboxRenderer.h"
#include "Renderer/Pipeline/SpriteRenderer.h"
#include "Renderer/Pipeline/StringRenderer2D.h"
#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/Buffer/UBO.h"

namespace he
{
  namespace xBar
  {
    class SkinnedGeometryContainer;
    class StaticGeometryContainer;
    class BillboardContainer;
  }

	namespace renderer
	{
    class GRAPHICAPI RenderManager : public util::SingletonBehaviour
    {
    public:

      RenderManager();
	    ~RenderManager();

      void setClearColor(he::util::Vector<float, 4> color) const;

      void resizeRenderWindow(unsigned int width, unsigned int height);

      void setBackfaceCulling(GLenum cullingMode) const;

      void setWireframe(bool wireFrame) const;

      const size_t getMaxSpriteLayer() const;

      void enableSkybox(util::SingletonManager *singletonManager, util::ResourceHandle skyboxShaderHandle, util::ResourceHandle skyboxTextureHandles[6]);
      void disableSkybox();

      void initialize(const RenderOptions& options, util::SingletonManager *singletonManager, unsigned char maxLayer,
        util::ResourceHandle billboardShaderHandle, 
        util::ResourceHandle spriteShaderHandle, 
        util::ResourceHandle stringShaderHandle, 
        util::ResourceHandle frustumCullingShaderHandle,
        util::ResourceHandle gBufferShaderHandle);

      void setNearFarPlane(float near, float far);

	    void render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition) const;

      void addRenderComponent(const Sprite* sprite);
      void addRenderComponent(const StringTexture2D* string);
      void addRenderComponent(const xBar::BillboardContainer& billboard);
      void addRenderComponent(const xBar::StaticGeometryContainer& staticGeometry);
      void addRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry);
      void addRenderComponent(const xBar::LightContainer& light);
      void addRenderComponent(const xBar::ParticleContainer& particle);

      void removeRenderComponent(const Sprite* sprite);
      void removeRenderComponent(const StringTexture2D* string);
      void removeRenderComponent(const xBar::BillboardContainer& billboard);
      void removeRenderComponent(const xBar::StaticGeometryContainer& staticGeometry);
      void removeRenderComponent(const xBar::SkinnedGeometryContainer& staticGeometry);
      void removeRenderComponent(const xBar::LightContainer& light);
      void removeRenderComponent(const xBar::ParticleContainer& particle);

    private:

	    RenderManager(const RenderManager&);
      RenderManager& operator=(const RenderManager&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      UBO m_cameraParameterUBO;

      bool m_skyboxRendering;

      RenderOptions m_options;

      GBuffer m_gBuffer;
      GeometryRenderer m_geometryRasterizer;
      BillboardRenderer m_billboardRenderer;
      SkyboxRenderer m_skyboxRenderer;
      SpriteRenderer m_spriteRenderer;
      StringRenderer2D m_stringRenderer;

      std::list<xBar::LightContainer> m_renderLight;
      std::list<xBar::ParticleContainer> m_renderParticle;

      GLfloat m_aspectRatio;
    };
	}
}

#endif
