#ifndef LIGHTRENDERER_H_
#define LIGHTRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

#include <Shader/ShaderContainer.h>

#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Buffer/UBO.h"
#include "Renderer/Buffer/TBO.h"
#include "Renderer/Pipeline/Renderquad.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
    class EventManager;
  }

  namespace xBar
  {
    class LightContainer;
    class ShadowLightContainer;
  }

  namespace renderer
  {
    class RenderOptions;

    class LightRenderer
    {
    public:

      LightRenderer();
      ~LightRenderer();

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer();
      void render(util::SharedPointer<db::Texture2D> depthMap, util::SharedPointer<db::Texture2D> albedoMap, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap);

      void setShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex);
      void unsetShadowMap(unsigned int bindingPoint);

      void setReflectiveShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex);
      void unsetReflectiveShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex);

      void addLight(const xBar::LightContainer& light);
      void removeLight(const xBar::LightContainer& light);

      void addShadowLight(const xBar::ShadowLightContainer& light);
      void removeShadowLight(const xBar::ShadowLightContainer& light);

      void addReflectiveShadowLight(const xBar::ShadowLightContainer& light);
      void removeReflectiveShadowLight(const xBar::ShadowLightContainer& light);

      void clear() const;

      unsigned int getShadowLightNumber() const;

      unsigned int getReflectiveShadowLightNumber() const;

      util::SharedPointer<db::Texture2D> getLightTexture() const;

      util::SharedPointer<db::Texture3D> getShadowMaps() const;

      util::SharedPointer<db::Texture2D> getDebugFullResReflectiveShadowPosMaps() const;
      util::SharedPointer<db::Texture2D> getDebugFullResReflectiveShadowNormalAreaMaps() const;
      util::SharedPointer<db::Texture2D> getDebugFullResReflectiveShadowLuminousFluxMaps() const;

      util::SharedPointer<db::Texture3D> getReflectiveShadowPosMaps() const;
      util::SharedPointer<db::Texture3D> getReflectiveShadowNormalAreaMaps() const;
      util::SharedPointer<db::Texture3D> getReflectiveShadowLuminousFluxMaps() const;
      const GPUImmutableBuffer& getReflectiveShadowLights() const;

    private:
      
      LightRenderer(const LightRenderer&);
      LightRenderer& operator=(const LightRenderer&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      sh::ComputeShaderHandle m_reflectiveShadowMapDownsamplingHandle;
      sh::RenderShaderHandle m_directLightShaderHandle;
      
      std::list<const xBar::LightContainer> m_lights;
      GPUImmutableBuffer m_lightBuffer;

      std::list<const xBar::ShadowLightContainer> m_shadowLights;
      util::SharedPointer<db::Texture3D> m_shadowMaps;
      GPUImmutableBuffer m_shadowedLightBuffer;
      Renderquad m_renderShadowMapsQuad;

      std::list<const xBar::ShadowLightContainer> m_reflectiveShadowLights;
      util::SharedPointer<db::Texture2D> m_fullResReflectiveShadowDepthMap;
      util::SharedPointer<db::Texture2D> m_fullResReflectiveShadowPosMap;
      util::SharedPointer<db::Texture2D> m_fullResReflectiveShadowNormalAreaMap;
      util::SharedPointer<db::Texture2D> m_fullResReflectiveShadowLuminousFluxMap;

      util::SharedPointer<db::Texture3D> m_reflectiveShadowPosMaps;
      util::SharedPointer<db::Texture3D> m_reflectiveShadowNormalAreaMaps;
      util::SharedPointer<db::Texture3D> m_reflectiveShadowLuminousFluxMaps;
      GPUImmutableBuffer m_reflectiveShadowedLightBuffer;
      Renderquad m_renderReflectiveShadowMapsQuad;

      Renderquad m_renderLightMapQuad;
      util::SharedPointer<db::Texture2D> m_lightTexture;

      GLint m_maxShadowMapsPerTextureArray;
      GLint m_maxColorAttachments;//WHAT IS THIS?!?

      bool m_lightNumberChanged;
      bool m_shadowLightNumberChanged;
      bool m_reflectiveShadowLightNumberChanged;
    };
  }
}

#endif
