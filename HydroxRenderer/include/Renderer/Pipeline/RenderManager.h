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
#include "Renderer/Pipeline/GeometryRenderer.h"
#include "Renderer/Pipeline/BillboardRenderer.h"
#include "Renderer/Pipeline/SkyboxRenderer.h"
#include "Renderer/Pipeline/SpriteRenderer.h"
#include "Renderer/Pipeline/StringRenderer2D.h"

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

      void setClearColor(he::util::Vector<float, 4> color);

      void resizeRenderWindow(unsigned int width, unsigned int height);

      void setBackfaceCulling(GLenum cullingMode);

      void setWireframe(bool wireFrame);

      const size_t getMaxSpriteLayer() const;

      void enableSkybox(util::SingletonManager *singletonManager, util::ResourceHandle skyboxShaderHandle, util::ResourceHandle skyboxTextureHandles[6]);
      void disableSkybox();

      void initialize(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, GLfloat aspectRatio, unsigned char maxLayer,
        util::ResourceHandle billboardShaderHandle, 
        util::ResourceHandle spriteShaderHandle, 
        util::ResourceHandle stringShaderHandle, 
        util::ResourceHandle frustumCullingShaderHandle);

	    void render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition);

      void addRenderComponent(Sprite* sprite);
      void addRenderComponent(StringTexture2D* string);
      void addRenderComponent(xBar::BillboardContainer& billboard);
      void addRenderComponent(xBar::StaticGeometryContainer& staticGeometry);
      void addRenderComponent(xBar::SkinnedGeometryContainer& skinnedGeometry);
      void addRenderComponent(xBar::LightContainer& light);
      void addRenderComponent(xBar::ParticleContainer& particle);

      void removeRenderComponent(Sprite* sprite);
      void removeRenderComponent(StringTexture2D* string);
      void removeRenderComponent(xBar::BillboardContainer& billboard);
      void removeRenderComponent(xBar::StaticGeometryContainer& staticGeometry);
      void removeRenderComponent(xBar::SkinnedGeometryContainer& staticGeometry);
      void removeRenderComponent(xBar::LightContainer& light);
      void removeRenderComponent(xBar::ParticleContainer& particle);

    private:

	    RenderManager(const RenderManager&){}

      void registerRenderComponentSlots(util::EventManager *eventManager);

      UBO m_cameraParameterUBO;

      bool m_skyboxRendering;

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
