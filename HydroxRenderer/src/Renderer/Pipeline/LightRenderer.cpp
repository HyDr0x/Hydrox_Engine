#include "Renderer/Pipeline/LightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <XBar/LightContainer.h>

#include "Renderer/Resources/RenderShader.h"

namespace he
{
  namespace renderer
  {
    LightRenderer::LightRenderer()
    {
    }

    LightRenderer::~LightRenderer()
    {
      m_lights.clear();
    }

    void LightRenderer::initialize(util::SingletonManager *singletonManager, util::ResourceHandle directLightShaderHandle)
    {
      m_lightManager = singletonManager->getService<LightManager>();

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_directLightShaderHandle = directLightShaderHandle;
    }

    void LightRenderer::render() const
    {

    }

    void LightRenderer::addRenderComponent(const xBar::LightContainer& light)
    {
      m_lights.push_back(light);
    }

    void LightRenderer::removeRenderComponent(const xBar::LightContainer& light)
    {
      m_lights.remove(light);
    }

    void LightRenderer::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void(*)(const xBar::LightContainer& light)>(util::EventManager::OnAddLightNode);
      eventManager->addSlotToSignal<LightRenderer, void(*)(const xBar::LightContainer& light), void (LightRenderer::*)(const xBar::LightContainer& light)>(this, &LightRenderer::addRenderComponent, util::EventManager::OnAddLightNode);

      eventManager->addNewSignal<void(*)(const xBar::LightContainer& light)>(util::EventManager::OnRemoveLightNode);
      eventManager->addSlotToSignal<LightRenderer, void(*)(const xBar::LightContainer& light), void (LightRenderer::*)(const xBar::LightContainer& light)>(this, &LightRenderer::removeRenderComponent, util::EventManager::OnRemoveLightNode);
    }
  }
}
