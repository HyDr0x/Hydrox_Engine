#include "Renderer/Pipeline/LightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Timer/Timer.h>

#include <DataBase/Light.h>
#include <DataBase/ResourceManager.hpp>

#include <XBar/LightContainer.h>
#include <XBar/ShadowLightContainer.h>

#include "Renderer/Pipeline/RenderOptions.h"
#include "Renderer/OpenGLDebugLogger/OpenGLDebugLogManager.h"

namespace he
{
  namespace renderer
  {
    LightRenderer::LightRenderer() : 
      m_lightNumberChanged(false), 
      m_shadowLightNumberChanged(false),
      m_reflectiveShadowLightNumberChanged(false)
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
      
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_reflectiveShadowMapDownsamplingHandle = sh::ShaderContainer::REFLECTIVESHADOWMAPDOWNSAMPLING;
      m_directLightShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::DIRECTLIGHT, sh::ShaderSlotFlags(8192));

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_lightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::Light) * m_options->lightNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_shadowedLightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::ShadowLight) * m_options->lightNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_reflectiveShadowedLightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(db::ShadowLight) * m_options->lightNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);

      util::SharedPointer<OpenGLDebugLogManager> oglDebugLogManager = singletonManager->getService<OpenGLDebugLogManager>();
      m_maxShadowMapsPerTextureArray = oglDebugLogManager->getCapabilities(OpenGLDebugLogManager::MAX_ARRAY_TEXTURE_LAYERS);
      m_maxColorAttachments = oglDebugLogManager->getCapabilities(OpenGLDebugLogManager::MAX_COLOR_ATTACHMENTS);

      m_lightTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_renderLightMapQuad.setRenderTargets(1, m_lightTexture);

      m_fullResReflectiveShadowPosMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->shadowMapWidth, m_options->shadowMapWidth, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_fullResReflectiveShadowNormalAreaMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->shadowMapWidth, m_options->shadowMapWidth, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_fullResReflectiveShadowLuminousFluxMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->shadowMapWidth, m_options->shadowMapWidth, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));

      m_minificatedReflectiveShadowPosMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->reflectiveShadowMapWidth, m_options->shadowMapWidth, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_minificatedReflectiveShadowNormalAreaMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->reflectiveShadowMapWidth, m_options->shadowMapWidth, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_minificatedReflectiveShadowLuminousFluxMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->reflectiveShadowMapWidth, m_options->shadowMapWidth, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
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

        m_shadowMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, m_shadowLights.size(), GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));

        m_renderShadowMapsQuad.setRenderTargets3D(m_shadowMaps, 0, 0);
      }

      if(m_reflectiveShadowLightNumberChanged)
      {
        m_reflectiveShadowLightNumberChanged = false;

        m_reflectiveShadowDepthMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->shadowMapWidth, m_options->shadowMapWidth, m_reflectiveShadowLights.size(), GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
        
        m_reflectiveShadowPosMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->reflectiveShadowMapWidth, m_options->reflectiveShadowMapWidth, m_reflectiveShadowLights.size(), GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
        m_reflectiveShadowNormalAreaMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->reflectiveShadowMapWidth, m_options->reflectiveShadowMapWidth, m_reflectiveShadowLights.size(), GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
        m_reflectiveShadowLuminousFluxMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->reflectiveShadowMapWidth, m_options->reflectiveShadowMapWidth, m_reflectiveShadowLights.size(), GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
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

    void LightRenderer::render(util::SharedPointer<db::Texture2D> depthMap, util::SharedPointer<db::Texture2D> albedoMap, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_directLightShaderHandle);

      shader.useShader();

      GLuint lightNumber = m_lights.size();
      sh::RenderShader::setUniform(6, GL_UNSIGNED_INT, &lightNumber);

      GLuint shadowLightNumber = m_shadowLights.size();
      sh::RenderShader::setUniform(7, GL_UNSIGNED_INT, &shadowLightNumber);

      GLuint reflectiveShadowLightNumber = m_reflectiveShadowLights.size();
      sh::RenderShader::setUniform(8, GL_UNSIGNED_INT, &reflectiveShadowLightNumber);

      m_lightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_shadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_reflectiveShadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_renderLightMapQuad.setWriteFrameBuffer();
      
      depthMap->setTexture(0, 0);
      albedoMap->setTexture(1, 1);
      normalMap->setTexture(2, 2);
      materialMap->setTexture(3, 3);
      if(m_shadowMaps) m_shadowMaps->setTexture(4, 4);
      if(m_reflectiveShadowDepthMaps) m_reflectiveShadowDepthMaps->setTexture(5, 5);

      m_renderLightMapQuad.render();

      if(m_reflectiveShadowDepthMaps) m_reflectiveShadowDepthMaps->unsetTexture(5);
      if(m_shadowMaps) m_shadowMaps->unsetTexture(4);
      materialMap->unsetTexture(3);
      normalMap->unsetTexture(2);
      albedoMap->unsetTexture(1);
      depthMap->unsetTexture(0);

      m_renderLightMapQuad.unsetWriteFrameBuffer();
      m_shadowedLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_lightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      shader.useNoShader();
    }

    void LightRenderer::setShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex)
    {
      m_renderShadowMapsQuad.clearTargets(1.0f, util::vec4f::identity(), std::vector<unsigned int>(1, shadowMapIndex));
      m_renderShadowMapsQuad.setDepthLayer(shadowMapIndex);
      m_renderShadowMapsQuad.setWriteFrameBuffer();
      m_shadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
    }

    void LightRenderer::unsetShadowMap(unsigned int bindingPoint)
    {
      m_shadowedLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
      m_renderShadowMapsQuad.unsetWriteFrameBuffer();
    }

    void LightRenderer::setReflectiveShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex)
    {
      m_renderReflectiveShadowMapsQuad.setRenderTargets(m_reflectiveShadowDepthMaps, shadowMapIndex, 3, m_fullResReflectiveShadowPosMap, m_fullResReflectiveShadowNormalAreaMap, m_fullResReflectiveShadowLuminousFluxMap);

      std::vector<unsigned int> texturendices(3);
      texturendices[0] = shadowMapIndex;
      texturendices[1] = shadowMapIndex + m_reflectiveShadowLights.size();
      texturendices[2] = shadowMapIndex + 2 * m_reflectiveShadowLights.size();
      m_renderReflectiveShadowMapsQuad.clearTargets(1.0f, util::vec4f::identity(), texturendices);
      m_renderReflectiveShadowMapsQuad.setWriteFrameBuffer(texturendices);

      m_reflectiveShadowedLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
    }

    void LightRenderer::unsetReflectiveShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex)
    {
      m_reflectiveShadowedLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, bindingPoint);
      m_renderReflectiveShadowMapsQuad.unsetWriteFrameBuffer();

      {
        //CPUTIMER("MainloopCPUTimer", 0)
        //GPUTIMER("MainloopOGLTimer", 1)

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        const sh::ComputeShader& shader = m_shaderContainer->getComputeShader(m_reflectiveShadowMapDownsamplingHandle);

        shader.useShader();

        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT, &m_options->shadowMapWidth);
        sh::ComputeShader::setUniform(2, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);
        unsigned int minification = m_options->shadowMapWidth / m_options->reflectiveShadowMapWidth;
        sh::ComputeShader::setUniform(3, GL_UNSIGNED_INT, &minification);

        {
          m_fullResReflectiveShadowPosMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
          m_fullResReflectiveShadowNormalAreaMap->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
          m_fullResReflectiveShadowLuminousFluxMap->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);

          m_minificatedReflectiveShadowPosMap->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA32F);
          m_minificatedReflectiveShadowNormalAreaMap->bindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA32F);
          m_minificatedReflectiveShadowLuminousFluxMap->bindImageTexture(5, 0, GL_WRITE_ONLY, GL_RGBA32F);

          util::vec2i sampleDirection(1, 0);
          sh::ComputeShader::setUniform(0, GL_INT_VEC2, &sampleDirection[0]);
 
          sh::ComputeShader::dispatchComputeShader(1024, 1, 1);

          m_minificatedReflectiveShadowLuminousFluxMap->unbindImageTexture(5, 0, GL_WRITE_ONLY, GL_RGBA32F);
          m_minificatedReflectiveShadowNormalAreaMap->unbindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA32F);
          m_minificatedReflectiveShadowPosMap->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA32F);

          m_fullResReflectiveShadowLuminousFluxMap->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
          m_fullResReflectiveShadowNormalAreaMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
          m_fullResReflectiveShadowPosMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
        }

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        {
          m_minificatedReflectiveShadowPosMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
          m_minificatedReflectiveShadowNormalAreaMap->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
          m_minificatedReflectiveShadowLuminousFluxMap->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);

          m_reflectiveShadowPosMaps->bindImageTexture(3, 0, GL_FALSE, shadowMapIndex, GL_WRITE_ONLY, GL_RGBA32F);
          m_reflectiveShadowNormalAreaMaps->bindImageTexture(4, 0, GL_FALSE, shadowMapIndex, GL_WRITE_ONLY, GL_RGBA32F);
          m_reflectiveShadowLuminousFluxMaps->bindImageTexture(5, 0, GL_FALSE, shadowMapIndex, GL_WRITE_ONLY, GL_RGBA32F);

          util::vec2i sampleDirection(0, 1);
          sh::ComputeShader::setUniform(0, GL_INT_VEC2, &sampleDirection[0]);

          sh::ComputeShader::dispatchComputeShader(1024, 1, 1);

          m_reflectiveShadowLuminousFluxMaps->unbindImageTexture(5, 0, GL_FALSE, shadowMapIndex, GL_WRITE_ONLY, GL_RGBA32F);
          m_reflectiveShadowNormalAreaMaps->unbindImageTexture(4, 0, GL_FALSE, shadowMapIndex, GL_WRITE_ONLY, GL_RGBA32F);
          m_reflectiveShadowPosMaps->unbindImageTexture(3, 0, GL_FALSE, shadowMapIndex, GL_WRITE_ONLY, GL_RGBA32F);

          m_minificatedReflectiveShadowLuminousFluxMap->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
          m_minificatedReflectiveShadowNormalAreaMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
          m_minificatedReflectiveShadowPosMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
        }

        shader.useNoShader();
      }
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

      if(m_shadowLights.size() < m_maxShadowMapsPerTextureArray)
      {
        m_shadowLights.push_back(light);
        m_shadowLightNumberChanged = true;
      }
    }

    void LightRenderer::removeShadowLight(const xBar::ShadowLightContainer& light)
    {
      if(m_shadowLights.size() > 0)
      {
        m_shadowLights.remove(light);
        m_shadowLightNumberChanged = true;
      }
    }

    void LightRenderer::addReflectiveShadowLight(const xBar::ShadowLightContainer& light)
    {
      assert(m_reflectiveShadowLights.size() + 1 < m_options->lightNumber);

      if(m_reflectiveShadowLights.size() < m_maxShadowMapsPerTextureArray)
      {
        m_reflectiveShadowLights.push_back(light);
        m_reflectiveShadowLightNumberChanged = true;
      }
    }

    void LightRenderer::removeReflectiveShadowLight(const xBar::ShadowLightContainer& light)
    {
      if(m_reflectiveShadowLights.size() > 0)
      {
        m_reflectiveShadowLights.remove(light);
        m_reflectiveShadowLightNumberChanged = true;
      }
    }

    void LightRenderer::clear() const
    {
      m_renderLightMapQuad.clearTargets(1.0f, std::vector<util::vec4f>(1, util::vec4f(0.0f, 0.0f, 0.0f, 0.0f)), false);
    }

    unsigned int LightRenderer::getShadowLightNumber() const
    {
      return m_shadowLights.size();
    }

    unsigned int LightRenderer::getReflectiveShadowLightNumber() const
    {
      return m_reflectiveShadowLights.size();
    }

    util::SharedPointer<db::Texture2D> LightRenderer::getLightTexture() const
    {
      return m_lightTexture;
    }

    util::SharedPointer<db::Texture3D> LightRenderer::getShadowMaps() const
    {
      return m_shadowMaps;
    }

    util::SharedPointer<db::Texture2D> LightRenderer::getDebugFullResReflectiveShadowPosMaps() const
    {
      return m_minificatedReflectiveShadowPosMap;
    }

    util::SharedPointer<db::Texture2D> LightRenderer::getDebugFullResReflectiveShadowNormalAreaMaps() const
    {
      return m_minificatedReflectiveShadowNormalAreaMap;
    }

    util::SharedPointer<db::Texture2D> LightRenderer::getDebugFullResReflectiveShadowLuminousFluxMaps() const
    {
      return m_minificatedReflectiveShadowLuminousFluxMap;
    }

    util::SharedPointer<db::Texture3D> LightRenderer::getReflectiveShadowPosMaps() const
    {
      return m_reflectiveShadowPosMaps;
    }

    util::SharedPointer<db::Texture3D> LightRenderer::getReflectiveShadowNormalAreaMaps() const
    {
      return m_reflectiveShadowNormalAreaMaps;
    }

    util::SharedPointer<db::Texture3D> LightRenderer::getReflectiveShadowLuminousFluxMaps() const
    {
      return m_reflectiveShadowLuminousFluxMaps;
    }

    const GPUImmutableBuffer& LightRenderer::getReflectiveShadowLights() const
    {
      return m_reflectiveShadowedLightBuffer;
    }

    void LightRenderer::registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager)
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
