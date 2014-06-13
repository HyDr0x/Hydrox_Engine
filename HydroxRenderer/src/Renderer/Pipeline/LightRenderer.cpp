#include "Renderer/Pipeline/LightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <DataBase/Light.h>
#include <DataBase/RenderShader.h>
#include <DataBase/ResourceManager.hpp>

#include <XBar/LightContainer.h>

namespace he
{
  namespace renderer
  {
    LightRenderer::LightRenderer() : m_lightNumberChanged(false)
    {
      m_lightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::Light) * 1024, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    LightRenderer::~LightRenderer()
    {
      m_lights.clear();
    }

    void LightRenderer::initialize(RenderOptions options, util::SingletonManager *singletonManager, util::ResourceHandle directLightShaderHandle)
    {
      m_singletonManager = singletonManager;

      registerRenderComponentSlots(m_singletonManager->getService<util::EventManager>());

      m_directLightShaderHandle = directLightShaderHandle;

      m_lightTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options.width, options.height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32, nullptr, false));

      m_renderQuad.setRenderTargets(options, 1, m_lightTexture);
    }

    void LightRenderer::render(RenderOptions options, util::SharedPointer<db::Texture2D> depthMap, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap)
    {
      if(m_lightNumberChanged)
      {
        m_lightBuffer.resizeBuffer(sizeof(db::Light) * m_lights.size(), nullptr);

        m_lightNumberChanged = false;
      }

      unsigned int lightIndex = 0;
      for(std::list<const xBar::LightContainer>::const_iterator it = m_lights.begin(); it != m_lights.end(); it++, lightIndex++)
      {
        m_lightBuffer.setData(lightIndex * sizeof(db::Light), sizeof(db::Light), it->getLightData());
      }

      m_renderQuad.setReadTextures(3, depthMap, normalMap, materialMap);

      m_renderQuad.clearTargets(1.0f, std::vector<util::Vector<float, 4>>(1, util::Vector<float, 4>(0, 0, 0, 0)));

      db::RenderShader *shader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_directLightShaderHandle);

      shader->useShader();

      GLuint lightNumber = m_lights.size();
      db::RenderShader::setUniform(3, GL_UNSIGNED_INT, &lightNumber);

      m_lightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_renderQuad.setWriteFrameBuffer();
      
      m_renderQuad.render();

      m_renderQuad.unsetWriteFrameBuffer();
      m_lightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      shader->useNoShader();
    }

    util::SharedPointer<db::Texture2D> LightRenderer::getLightTexture() const
    {
      return m_lightTexture;
    }

    void LightRenderer::addRenderComponent(const xBar::LightContainer& light)
    {
      m_lights.push_back(light);
      m_lightNumberChanged = true;
    }

    void LightRenderer::removeRenderComponent(const xBar::LightContainer& light)
    {
      m_lights.remove(light);
      m_lightNumberChanged = true;
    }

    void LightRenderer::clear() const
    {
      m_renderQuad.clearTargets(1.0f, util::Vector<float, 4>(1.0f, 1.0f, 1.0f, 1.0f));
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
