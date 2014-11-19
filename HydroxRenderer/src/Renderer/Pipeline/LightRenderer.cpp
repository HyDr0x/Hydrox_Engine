#include "Renderer/Pipeline/LightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <DataBase/Light.h>
#include <DataBase/RenderShader.h>
#include <DataBase/ResourceManager.hpp>

#include <XBar/LightContainer.h>
#include <XBar/ShadowLightContainer.h>

#include "Renderer/Pipeline/RenderOptions.h"
#include "Renderer/Pipeline/RenderShaderContainer.h"

namespace he
{
  namespace renderer
  {
    LightRenderer::LightRenderer() : 
      m_lightNumberChanged(false), 
      m_shadowLightNumberChanged(false)
    {
    }

    LightRenderer::~LightRenderer()
    {
      m_lights.clear();
      m_shadowLights.clear();
      m_reflectiveShadowLights.clear();
    }

    void LightRenderer::initialize(util::SingletonManager *singletonManager)
    {
      m_options = singletonManager->getService<RenderOptions>();
      m_singletonManager = singletonManager;

      registerRenderComponentSlots(m_singletonManager->getService<util::EventManager>());

      m_directLightShaderHandle = singletonManager->getService<RenderShaderContainer>()->directLightShaderHandle;

      m_lightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::Light) * m_options->lightNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_shadowedLightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::ShadowLight) * m_options->lightNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_reflectiveShadowedLightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::ShadowLight) * m_options->lightNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);

      glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &m_maxShadowMapsPerTextureArray);

      m_lightTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr, false));
      m_shadowDepthMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->shadowMapWidth, m_options->shadowMapWidth, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32, nullptr));

      m_renderLightMapQuad.setRenderTargets(1, m_lightTexture);
    }

    void LightRenderer::updateBuffer()
    {
      if(m_lightNumberChanged)
      {
        m_lightNumberChanged = false;
      }

      if(m_shadowLightNumberChanged)
      {
        m_shadowLightNumberChanged = false;

        m_renderShadowMapsQuad.setRenderTargets3D(m_shadowDepthMap, 1, m_shadowMaps);
      }

      if(m_relfectiveShadowLightNumberChanged)
      {
        m_relfectiveShadowLightNumberChanged = false;

        m_renderReflectiveShadowMapsQuad.setRenderTargets3D(m_shadowDepthMap, 3, m_shadowPosMaps, m_shadowNormalMaps, m_shadowLuminousFluxMaps);
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

      lightIndex = 0;
      for(std::list<const xBar::ShadowLightContainer>::const_iterator it = m_reflectiveShadowLights.begin(); it != m_reflectiveShadowLights.end(); it++, lightIndex++)
      {
        m_reflectiveShadowedLightBuffer.setData(lightIndex * sizeof(db::ShadowLight), sizeof(db::ShadowLight), (db::ShadowLight*)it->getLightData());
      }
    }

    void LightRenderer::render(util::SharedPointer<db::Texture2D> depthMap, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap)
    {
      m_renderLightMapQuad.clearTargets(1.0f, std::vector<util::vec4f>(1, util::vec4f(0, 0, 0, 0)));

      db::RenderShader *shader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_directLightShaderHandle);

      shader->useShader();

      GLuint lightNumber = m_lights.size();
      db::RenderShader::setUniform(5, GL_UNSIGNED_INT, &lightNumber);

      GLuint shadowLightNumber = m_shadowLights.size();
      db::RenderShader::setUniform(6, GL_UNSIGNED_INT, &shadowLightNumber);

      GLuint reflectiveShadowLightNumber = m_reflectiveShadowLights.size();
      db::RenderShader::setUniform(7, GL_UNSIGNED_INT, &reflectiveShadowLightNumber);

      m_lightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_shadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_reflectiveShadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_renderLightMapQuad.setWriteFrameBuffer();
      
      depthMap->setTexture(0, 0);
      normalMap->setTexture(1, 1);
      materialMap->setTexture(2, 2);
      if(m_shadowMaps) m_shadowMaps->setTexture(3, 3);
      if(m_shadowPosMaps) m_shadowPosMaps->setTexture(4, 4);

      m_renderLightMapQuad.render();

      if(m_shadowPosMaps) m_shadowPosMaps->unsetTexture();
      if(m_shadowMaps) m_shadowMaps->unsetTexture();
      materialMap->unsetTexture();
      normalMap->unsetTexture();
      depthMap->unsetTexture();

      m_renderLightMapQuad.unsetWriteFrameBuffer();
      m_reflectiveShadowedLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_shadowedLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_lightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      shader->useNoShader();
    }

    util::SharedPointer<db::Texture2D> LightRenderer::getLightTexture() const
    {
      return m_lightTexture;
    }

    void LightRenderer::setShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex)
    {
      m_renderShadowMapsQuad.clearTargets(1.0f, util::vec4f::identity(), std::vector<unsigned int>(1, shadowMapIndex));
      m_renderShadowMapsQuad.setWriteFrameBuffer(std::vector<unsigned int>(1, shadowMapIndex));
      m_shadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
    }

    void LightRenderer::unsetShadowMap(unsigned int bindingPoint)
    {
      m_shadowedLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
      m_renderShadowMapsQuad.unsetWriteFrameBuffer();
    }

    void LightRenderer::setReflectiveShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex)
    {
      std::vector<unsigned int> texturendices(3);
      texturendices[0] = shadowMapIndex;
      texturendices[1] = shadowMapIndex + m_reflectiveShadowLights.size();
      texturendices[2] = shadowMapIndex + 2 * m_reflectiveShadowLights.size();
      m_renderReflectiveShadowMapsQuad.clearTargets(1.0f, util::vec4f::identity(), texturendices);
      m_renderReflectiveShadowMapsQuad.setWriteFrameBuffer(texturendices);
      m_reflectiveShadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
    }

    void LightRenderer::unsetReflectiveShadowMap(unsigned int bindingPoint)
    {
      m_reflectiveShadowedLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
      m_renderReflectiveShadowMapsQuad.unsetWriteFrameBuffer();
    }

    void LightRenderer::addLight(const xBar::LightContainer& light)
    {
      assert(m_lights.size() + 1 < m_options->lightNumber);

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
      assert(m_shadowLights.size() + 1 < m_options->lightNumber);

      unsigned int shadowMapLayer = m_shadowMaps ? m_shadowMaps->getResolution()[2] : 0;

      if(shadowMapLayer < m_maxShadowMapsPerTextureArray)
      {
        shadowMapLayer++;
      }
      
      m_shadowMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, shadowMapLayer, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_R32F, GL_RED, 1, 32, nullptr));

      m_shadowLights.push_back(light);
      m_shadowLightNumberChanged = true;
    }

    void LightRenderer::removeShadowLight(const xBar::ShadowLightContainer& light)
    {
      unsigned int shadowMapLayer = m_shadowMaps ? m_shadowMaps->getResolution()[2] : 0;

      if(shadowMapLayer > 0)
      {
        m_shadowMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, shadowMapLayer - 1, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_R32F, GL_RED, 1, 32, nullptr));
      }

      m_shadowLights.remove(light);
      m_shadowLightNumberChanged = true;
    }

    void LightRenderer::addReflectiveShadowLight(const xBar::ShadowLightContainer& light)
    {
      assert(m_reflectiveShadowLights.size() + 1 < m_options->lightNumber);

      unsigned int shadowMapLayer = m_shadowPosMaps ? m_shadowPosMaps->getResolution()[2] : 0;

      if(shadowMapLayer < m_maxShadowMapsPerTextureArray)
      {
        shadowMapLayer++;
      }

      m_shadowPosMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, shadowMapLayer, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
      m_shadowNormalMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, shadowMapLayer, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
      m_shadowLuminousFluxMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, shadowMapLayer, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
      
      m_reflectiveShadowLights.push_back(light);
      m_relfectiveShadowLightNumberChanged = true;
    }

    void LightRenderer::removeReflectiveShadowLight(const xBar::ShadowLightContainer& light)
    {
      unsigned int shadowMapLayer = m_shadowPosMaps ? m_shadowPosMaps->getResolution()[2] : 0;

      if(shadowMapLayer > 0)
      {
        m_shadowPosMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, shadowMapLayer, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
        m_shadowNormalMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, shadowMapLayer, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
        m_shadowLuminousFluxMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, shadowMapLayer, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
      }

      m_reflectiveShadowLights.remove(light);
      m_relfectiveShadowLightNumberChanged = true;
    }

    void LightRenderer::clear() const
    {
      m_renderLightMapQuad.clearTargets(1.0f, std::vector<util::vec4f>(1, util::vec4f(1.0f, 1.0f, 1.0f, 1.0f)));
    }

    unsigned int LightRenderer::getShadowLightNumber() const
    {
      return m_shadowLights.size();
    }

    unsigned int LightRenderer::getReflectiveShadowLightNumber() const
    {
      return m_reflectiveShadowLights.size();
    }

    util::SharedPointer<db::Texture3D> LightRenderer::getShadowMaps() const
    {
      return m_shadowMaps;
    }

    util::SharedPointer<db::Texture3D> LightRenderer::getReflectiveShadowPosMaps() const
    {
      return m_shadowPosMaps;
    }

    util::SharedPointer<db::Texture3D> LightRenderer::getReflectiveShadowNormalMaps() const
    {
      return m_shadowNormalMaps;
    }

    util::SharedPointer<db::Texture3D> LightRenderer::getReflectiveShadowLuminousFluxMaps() const
    {
      return m_shadowLuminousFluxMaps;
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

      eventManager->addNewSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnAddReflectiveShadowLightNode);
      eventManager->addSlotToSignal<LightRenderer, void(*)(const xBar::ShadowLightContainer& light), void (LightRenderer::*)(const xBar::ShadowLightContainer& light)>(this, &LightRenderer::addReflectiveShadowLight, util::EventManager::OnAddReflectiveShadowLightNode);

      eventManager->addNewSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnRemoveReflectiveShadowLightNode);
      eventManager->addSlotToSignal<LightRenderer, void(*)(const xBar::ShadowLightContainer& light), void (LightRenderer::*)(const xBar::ShadowLightContainer& light)>(this, &LightRenderer::removeReflectiveShadowLight, util::EventManager::OnRemoveReflectiveShadowLightNode);
    }
  }
}
