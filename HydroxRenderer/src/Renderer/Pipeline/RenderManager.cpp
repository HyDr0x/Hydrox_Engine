#include "Renderer/Pipeline/RenderManager.h"

#include <XBar/BillboardContainer.h>
#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>
#include <XBar/LightContainer.h>
#include <XBar/ParticleContainer.h>

namespace he
{
	namespace renderer
	{
    RenderManager::RenderManager()
    {
    }

    RenderManager::~RenderManager()
    {
    }

    void RenderManager::setClearColor(he::util::Vector<float, 4> color)
    {
	    glClearColor(color[0], color[1], color[2], color[3]);
    }

    void RenderManager::resizeRenderWindow(unsigned int width, unsigned int height)
    {
      m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

      glViewport(0, 0, width, height);
    }

    void RenderManager::setBackfaceCulling(GLenum cullingMode)
    {
      glFrontFace(cullingMode);
    }

    void RenderManager::setWireframe(bool wireFrame)
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

    void RenderManager::initialize(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, GLfloat aspectRatio, size_t maxSpriteLayer, 
      util::ResourceHandle billboardShaderHandle, 
      util::ResourceHandle spriteShaderHandle, 
      util::ResourceHandle frustumCullingShaderHandle)
    {
      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_geometryRasterizer.initialize(maxMaterials, maxGeometry, maxBones, singletonManager, frustumCullingShaderHandle);
      m_billboardRenderer.initialize(singletonManager, billboardShaderHandle);
      m_spriteRenderer.initialize(singletonManager, maxSpriteLayer, spriteShaderHandle);

      m_aspectRatio = aspectRatio;

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 3 + sizeof(util::Vector<float, 4>), GL_DYNAMIC_DRAW);
    }

    void RenderManager::render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition)
    {
      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
      util::Vector<float, 4> eyeVec = util::Vector<float, 4>(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

      m_cameraParameterUBO.setData(&viewMatrix[0][0], 0, sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&projectionMatrix[0][0], sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&viewProjectionMatrix[0][0], 2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&eyeVec[0], 3 * sizeof(util::Matrix<float, 4>), sizeof(util::Vector<float, 4>));
      m_cameraParameterUBO.uploadData();
      m_cameraParameterUBO.bindBuffer(0);

      m_geometryRasterizer.rasterizeGeometry();
      m_billboardRenderer.render();
      m_spriteRenderer.render();
    }

    void RenderManager::addRenderComponent(Sprite *sprite)
    {
      m_spriteRenderer.addRenderComponent(sprite);
    }

    void RenderManager::addRenderComponent(xBar::BillboardContainer& billboard)
    {
      m_billboardRenderer.addRenderComponent(billboard);
    }

    void RenderManager::addRenderComponent(xBar::StaticGeometryContainer& staticGeometry)
    {
      m_geometryRasterizer.addRenderComponent(staticGeometry);
    }

    void RenderManager::addRenderComponent(xBar::SkinnedGeometryContainer& skinnedGeometry)
    {
      m_geometryRasterizer.addRenderComponent(skinnedGeometry);
    }

    void RenderManager::addRenderComponent(xBar::LightContainer& light)
    {
      m_renderLight.push_back(light.clone());
    }

    void RenderManager::addRenderComponent(xBar::ParticleContainer& particle)
    {
      m_renderParticle.push_back(particle.clone());
    }

    void RenderManager::removeRenderComponent(Sprite *sprite)
    {
      m_spriteRenderer.removeRenderComponent(sprite);
    }

    void RenderManager::removeRenderComponent(xBar::BillboardContainer& billboard)
    {
      m_billboardRenderer.removeRenderComponent(billboard);
    }

    void RenderManager::removeRenderComponent(xBar::StaticGeometryContainer& staticGeometry)
    {
      m_geometryRasterizer.removeRenderComponent(staticGeometry);
    }

    void RenderManager::removeRenderComponent(xBar::SkinnedGeometryContainer& skinnedGeometry)
    {
      m_geometryRasterizer.removeRenderComponent(skinnedGeometry);
    }

    void RenderManager::removeRenderComponent(xBar::LightContainer& light)
    {
      m_renderLight.remove(light.clone());
    }

    void RenderManager::removeRenderComponent(xBar::ParticleContainer& particle)
    {
      m_renderParticle.remove(particle.clone());
    }

    void RenderManager::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(xBar::LightContainer& light)>(util::EventManager::OnAddLightNode);
      eventManager->addSlotToSignal<RenderManager, void (*)(xBar::LightContainer& light), void (RenderManager::*)(xBar::LightContainer& light)>(this, &RenderManager::addRenderComponent, util::EventManager::OnAddLightNode);

      eventManager->addNewSignal<void (*)(xBar::ParticleContainer& particle)>(util::EventManager::OnAddParticleTransmitterNode);
      eventManager->addSlotToSignal<RenderManager, void (*)(xBar::ParticleContainer& particle), void (RenderManager::*)(xBar::ParticleContainer& particle)>(this, &RenderManager::addRenderComponent, util::EventManager::OnAddParticleTransmitterNode);

      eventManager->addNewSignal<void (*)(xBar::LightContainer& light)>(util::EventManager::OnRemoveLightNode);
      eventManager->addSlotToSignal<RenderManager, void (*)(xBar::LightContainer& light), void (RenderManager::*)(xBar::LightContainer& light)>(this, &RenderManager::removeRenderComponent, util::EventManager::OnRemoveLightNode);

      eventManager->addNewSignal<void (*)(xBar::ParticleContainer& particle)>(util::EventManager::OnRemoveParticleTransmitterNode);
      eventManager->addSlotToSignal<RenderManager, void (*)(xBar::ParticleContainer& particle), void (RenderManager::*)(xBar::ParticleContainer& particle)>(this, &RenderManager::removeRenderComponent, util::EventManager::OnRemoveParticleTransmitterNode);
    }
	}
}
