#include "Renderer/Pipeline/ParticleRenderer.h"

#include <DataBase/ShaderContainer.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <XBar/ParticleEmitterContainer.h>

#include <DataBase/RenderShader.h>

namespace he
{
  namespace renderer
  {
    ParticleRenderer::ParticleRenderer()
    {
    }

    ParticleRenderer::~ParticleRenderer()
    {
      m_particleEmitter.clear();
    }

    void ParticleRenderer::initialize(util::SingletonManager *singletonManager)
    {
      m_particleEmitterManager = singletonManager->getService<db::ParticleEmitterManager>();

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      //m_particleShaderHandle = singletonManager->getService<db::ShaderContainer>()->particleShaderHandle;
    }

    void ParticleRenderer::render() const
    {

    }

    void ParticleRenderer::addRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter)
    {
      m_particleEmitter.push_back(particleEmitter);
    }

    void ParticleRenderer::removeRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter)
    {
      m_particleEmitter.remove(particleEmitter);
    }

    void ParticleRenderer::registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager)
    {
      eventManager->addNewSignal<void(*)(const xBar::ParticleEmitterContainer& particleEmitter)>(util::EventManager::OnAddParticleNode);
      eventManager->addSlotToSignal<ParticleRenderer, void(*)(const xBar::ParticleEmitterContainer& particleEmitter), void (ParticleRenderer::*)(const xBar::ParticleEmitterContainer& particleEmitter)>(this, &ParticleRenderer::addRenderComponent, util::EventManager::OnAddParticleNode);

      eventManager->addNewSignal<void(*)(const xBar::ParticleEmitterContainer& particleEmitter)>(util::EventManager::OnRemoveParticleNode);
      eventManager->addSlotToSignal<ParticleRenderer, void(*)(const xBar::ParticleEmitterContainer& particleEmitter), void (ParticleRenderer::*)(const xBar::ParticleEmitterContainer& particleEmitter)>(this, &ParticleRenderer::removeRenderComponent, util::EventManager::OnRemoveParticleNode);
    }
  }
}
