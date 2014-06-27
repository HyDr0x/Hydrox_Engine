#include "Renderer/Pipeline/LightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <DataBase/Light.h>
#include <DataBase/RenderShader.h>
#include <DataBase/ResourceManager.hpp>

#include <XBar/LightContainer.h>
#include <XBar/ShadowLightContainer.h>

namespace he
{
  namespace renderer
  {
    LightRenderer::LightRenderer() : m_lightNumberChanged(false), m_shadowLightNumberChanged(false)
    {
      m_lightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::Light) * 1024, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_shadowedLightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::ShadowLight) * 1024, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);

      glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &m_maxShadowMapsPerTextureArray);
    }

    LightRenderer::~LightRenderer()
    {
      m_lights.clear();
    }

    void LightRenderer::initialize(RenderOptions options, util::SingletonManager *singletonManager, util::ResourceHandle directLightShaderHandle)
    {
      m_options = options;
      m_singletonManager = singletonManager;

      registerRenderComponentSlots(m_singletonManager->getService<util::EventManager>());

      m_directLightShaderHandle = directLightShaderHandle;

      m_lightTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options.width, m_options.height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr, false));

      m_renderLightMapQuad.setRenderTargets(1, m_lightTexture);
    }

    void LightRenderer::updateBuffer()
    {
      if(m_lightNumberChanged)
      {
        m_lightBuffer.resizeBuffer(sizeof(db::Light) * m_lights.size(), nullptr);

        m_lightNumberChanged = false;
      }

      if(m_shadowLightNumberChanged)
      {
        m_shadowedLightBuffer.resizeBuffer(sizeof(db::ShadowLight) * m_shadowLights.size(), nullptr);

        m_shadowLightNumberChanged = false;

        m_renderShadowMapsQuad.setRenderTargets(m_shadowMaps);
      }

      unsigned int lightIndex = 0;
      for(std::list<const xBar::LightContainer>::const_iterator it = m_lights.begin(); it != m_lights.end(); it++, lightIndex++)
      {
        m_lightBuffer.setData(lightIndex * sizeof(db::Light), sizeof(db::Light), it->getLightData());
      }

      lightIndex = 0;
      for(std::list<const xBar::ShadowLightContainer>::const_iterator it = m_shadowLights.begin(); it != m_shadowLights.end(); it++, lightIndex++)
      {
        m_shadowedLightBuffer.setData(lightIndex * sizeof(db::ShadowLight), sizeof(db::ShadowLight), (db::ShadowLight*)it->getLightData());
      }
    }

    void LightRenderer::render(util::SharedPointer<db::Texture2D> depthMap, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap)
    {
      m_renderLightMapQuad.setReadTextures(3, depthMap, normalMap, materialMap);

      m_renderLightMapQuad.clearTargets(1.0f, std::vector<util::Vector<float, 4>>(1, util::Vector<float, 4>(0, 0, 0, 0)));

      db::RenderShader *shader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_directLightShaderHandle);

      shader->useShader();

      GLuint lightNumber = m_lights.size();
      db::RenderShader::setUniform(3, GL_UNSIGNED_INT, &lightNumber);

      GLuint shadowLightNumber = m_shadowLights.size();
      db::RenderShader::setUniform(4, GL_UNSIGNED_INT, &shadowLightNumber);

      m_lightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_shadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_renderLightMapQuad.setWriteFrameBuffer();
      
      m_renderLightMapQuad.render();

      m_renderLightMapQuad.unsetWriteFrameBuffer();
      m_shadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_lightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      shader->useNoShader();
    }

    util::SharedPointer<db::Texture2D> LightRenderer::getLightTexture() const
    {
      return m_lightTexture;
    }

    void LightRenderer::setShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex)
    {
      m_renderShadowMapsQuad.setWriteFrameBuffer(std::vector<unsigned int>(1, shadowMapIndex));
      m_shadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
    }

    void LightRenderer::unsetShadowMap(unsigned int bindingPoint)
    {
      m_shadowedLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
      m_renderShadowMapsQuad.unsetWriteFrameBuffer();
    }

    void LightRenderer::addLight(const xBar::LightContainer& light)
    {
      m_lights.push_back(light);
      m_lightNumberChanged = true;
    }

    void LightRenderer::removeLight(const xBar::LightContainer& light)
    {
      m_lights.remove(light);
      m_lightNumberChanged = true;
    }

    void LightRenderer::addShadowLight(const xBar::ShadowLightContainer& light)
    {
      unsigned int shadowMapLayer = m_shadowMaps ? m_shadowMaps->getResolution()[2]  : 0;

      if(shadowMapLayer < m_maxShadowMapsPerTextureArray)
      {
        shadowMapLayer++;
      }
      
      m_shadowMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options.shadowMapWidth, m_options.shadowMapHeight, shadowMapLayer, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_R32F, GL_RED, 1, 32, nullptr));

      m_shadowLights.push_back(light);
      m_shadowLightNumberChanged = true;
    }

    void LightRenderer::removeShadowLight(const xBar::ShadowLightContainer& light)
    {
      unsigned int shadowMapLayer = m_shadowMaps ? m_shadowMaps->getResolution()[2] : 0;

      if(shadowMapLayer > 1)
      {
        m_shadowMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options.shadowMapWidth, m_options.shadowMapHeight, shadowMapLayer - 1, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_R32F, GL_RED, 1, 32, nullptr));
      }

      m_shadowLights.remove(light);
      m_shadowLightNumberChanged = true;
    }

    void LightRenderer::clear() const
    {
      m_renderLightMapQuad.clearTargets(1.0f, util::Vector<float, 4>(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void LightRenderer::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void(*)(const xBar::LightContainer& light)>(util::EventManager::OnAddLightNode);
      eventManager->addSlotToSignal<LightRenderer, void(*)(const xBar::LightContainer& light), void (LightRenderer::*)(const xBar::LightContainer& light)>(this, &LightRenderer::addLight, util::EventManager::OnAddLightNode);

      eventManager->addNewSignal<void(*)(const xBar::LightContainer& light)>(util::EventManager::OnRemoveLightNode);
      eventManager->addSlotToSignal<LightRenderer, void(*)(const xBar::LightContainer& light), void (LightRenderer::*)(const xBar::LightContainer& light)>(this, &LightRenderer::removeLight, util::EventManager::OnRemoveLightNode);

      eventManager->addNewSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnAddShadowLightNode);
      eventManager->addSlotToSignal<LightRenderer, void(*)(const xBar::ShadowLightContainer& light), void (LightRenderer::*)(const xBar::ShadowLightContainer& light)>(this, &LightRenderer::addShadowLight, util::EventManager::OnAddShadowLightNode);

      eventManager->addNewSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnRemoveShadowLightNode);
      eventManager->addSlotToSignal<LightRenderer, void(*)(const xBar::ShadowLightContainer& light), void (LightRenderer::*)(const xBar::ShadowLightContainer& light)>(this, &LightRenderer::removeShadowLight, util::EventManager::OnRemoveShadowLightNode);
    }
  }
}
