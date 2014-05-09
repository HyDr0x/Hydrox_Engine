#include "Renderer/Pipeline/RenderManager.h"

#include <XBar/BillboardContainer.h>
#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Pipeline/RenderingOptions.h"

namespace he
{
	namespace renderer
	{
    RenderManager::RenderManager() : m_geometryRasterizer(m_options), m_skyboxRendering(false)
    {
    }

    RenderManager::~RenderManager()
    {
    }

    void RenderManager::setClearColor(he::util::Vector<float, 4> color) const
    {
	    glClearColor(color[0], color[1], color[2], color[3]);
    }

    void RenderManager::resizeRenderWindow(unsigned int width, unsigned int height)
    {
      m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

      glViewport(0, 0, width, height);
    }

    void RenderManager::setBackfaceCulling(GLenum cullingMode) const
    {
      glFrontFace(cullingMode);
    }

    void RenderManager::setWireframe(bool wireFrame) const
    {
      if(wireFrame)
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      else
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
    }

    void RenderManager::enableSkybox(util::SingletonManager *singletonManager, util::ResourceHandle skyboxShaderHandle, util::ResourceHandle skyboxTextureHandles[6])
    {
      m_skyboxRendering = true;
      m_skyboxRenderer.initialize(singletonManager, skyboxShaderHandle, skyboxTextureHandles);
    }

    void RenderManager::disableSkybox()
    {
      m_skyboxRendering = false;
    }

    void RenderManager::initialize(const RenderOptions& options, util::SingletonManager *singletonManager, unsigned char maxLayer,
      util::ResourceHandle billboardShaderHandle, 
      util::ResourceHandle spriteShaderHandle, 
      util::ResourceHandle stringShaderHandle,
      util::ResourceHandle frustumCullingShaderHandle)
    {
      m_options = options;

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_geometryRasterizer.initialize(singletonManager, frustumCullingShaderHandle);
      m_billboardRenderer.initialize(singletonManager, billboardShaderHandle);
      m_spriteRenderer.initialize(singletonManager, spriteShaderHandle, maxLayer);
      m_stringRenderer.initialize(singletonManager, stringShaderHandle, maxLayer);

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 3 + sizeof(util::Vector<float, 4>), GL_DYNAMIC_DRAW);
    }

    void RenderManager::render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition) const
    {
      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
      util::Vector<float, 4> eyeVec = util::Vector<float, 4>(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

      m_cameraParameterUBO.setData(0, sizeof(util::Matrix<float, 4>), &viewMatrix[0][0]);
      m_cameraParameterUBO.setData(sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &projectionMatrix[0][0]);
      m_cameraParameterUBO.setData(2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &viewProjectionMatrix[0][0]);
      m_cameraParameterUBO.setData(3 * sizeof(util::Matrix<float, 4>), sizeof(util::Vector<float, 4>), &eyeVec[0]);
      m_cameraParameterUBO.uploadData();
      m_cameraParameterUBO.bindBuffer(0);

      m_geometryRasterizer.rasterizeGeometry();
      m_billboardRenderer.render();

      if (m_skyboxRendering)
      {
        m_skyboxRenderer.render();
      }

      m_spriteRenderer.render();
      m_stringRenderer.render();

      m_cameraParameterUBO.unBindBuffer(0);
    }

    void RenderManager::addRenderComponent(const Sprite *sprite)
    {
      m_spriteRenderer.addRenderComponent(sprite);
    }

    void RenderManager::addRenderComponent(const StringTexture2D *string)
    {
      m_stringRenderer.addRenderComponent(string);
    }

    void RenderManager::addRenderComponent(const xBar::BillboardContainer& billboard)
    {
      m_billboardRenderer.addRenderComponent(billboard);
    }

    void RenderManager::addRenderComponent(const xBar::StaticGeometryContainer& staticGeometry)
    {
      m_geometryRasterizer.addRenderComponent(staticGeometry);
    }

    void RenderManager::addRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry)
    {
      m_geometryRasterizer.addRenderComponent(skinnedGeometry);
    }

    void RenderManager::addRenderComponent(const xBar::LightContainer& light)
    {
      m_renderLight.push_back(light);
    }

    void RenderManager::addRenderComponent(const xBar::ParticleContainer& particle)
    {
      m_renderParticle.push_back(particle);
    }

    void RenderManager::removeRenderComponent(const Sprite *sprite)
    {
      m_spriteRenderer.removeRenderComponent(sprite);
    }

    void RenderManager::removeRenderComponent(const StringTexture2D *string)
    {
      m_stringRenderer.removeRenderComponent(string);
    }

    void RenderManager::removeRenderComponent(const xBar::BillboardContainer& billboard)
    {
      m_billboardRenderer.removeRenderComponent(billboard);
    }

    void RenderManager::removeRenderComponent(const xBar::StaticGeometryContainer& staticGeometry)
    {
      m_geometryRasterizer.removeRenderComponent(staticGeometry);
    }

    void RenderManager::removeRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry)
    {
      m_geometryRasterizer.removeRenderComponent(skinnedGeometry);
    }

    void RenderManager::removeRenderComponent(const xBar::LightContainer& light)
    {
      m_renderLight.remove(light);
    }

    void RenderManager::removeRenderComponent(const xBar::ParticleContainer& particle)
    {
      m_renderParticle.remove(particle);
    }

    void RenderManager::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(const xBar::LightContainer& light)>(util::EventManager::OnAddLightNode);
      eventManager->addSlotToSignal<RenderManager, void (*)(const xBar::LightContainer& light), void (RenderManager::*)(const xBar::LightContainer& light)>(this, &RenderManager::addRenderComponent, util::EventManager::OnAddLightNode);

      eventManager->addNewSignal<void (*)(const xBar::ParticleContainer& particle)>(util::EventManager::OnAddParticleTransmitterNode);
      eventManager->addSlotToSignal<RenderManager, void (*)(const xBar::ParticleContainer& particle), void (RenderManager::*)(const xBar::ParticleContainer& particle)>(this, &RenderManager::addRenderComponent, util::EventManager::OnAddParticleTransmitterNode);

      eventManager->addNewSignal<void (*)(const xBar::LightContainer& light)>(util::EventManager::OnRemoveLightNode);
      eventManager->addSlotToSignal<RenderManager, void (*)(const xBar::LightContainer& light), void (RenderManager::*)(const xBar::LightContainer& light)>(this, &RenderManager::removeRenderComponent, util::EventManager::OnRemoveLightNode);

      eventManager->addNewSignal<void (*)(const xBar::ParticleContainer& particle)>(util::EventManager::OnRemoveParticleTransmitterNode);
      eventManager->addSlotToSignal<RenderManager, void (*)(const xBar::ParticleContainer& particle), void (RenderManager::*)(const xBar::ParticleContainer& particle)>(this, &RenderManager::removeRenderComponent, util::EventManager::OnRemoveParticleTransmitterNode);
    }
	}
}
