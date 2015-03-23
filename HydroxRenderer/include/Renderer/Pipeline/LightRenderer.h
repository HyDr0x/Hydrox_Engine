#ifndef LIGHTRENDERER_H_
#define LIGHTRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/Texture2D.h>

#include "Renderer/Buffer/GPUImmutableBuffer.h"
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
      void render(util::SharedPointer<db::Texture2D> depthMap, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap);

      void setShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex);
      void unsetShadowMap(unsigned int bindingPoint);

      void setReflectiveShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex);
      void unsetReflectiveShadowMap(unsigned int bindingPoint);

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

      util::SharedPointer<db::Texture3D> getReflectiveShadowPosMaps() const;
      util::SharedPointer<db::Texture3D> getReflectiveShadowNormalMaps() const;
      util::SharedPointer<db::Texture3D> getReflectiveShadowLuminousFluxMaps() const;
      const GPUImmutableBuffer& getReflectiveShadowLights() const;

    private:
      
      LightRenderer(const LightRenderer&);
      LightRenderer& operator=(const LightRenderer&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      util::SharedPointer<RenderOptions> m_options;

      util::SingletonManager *m_singletonManager;

      

      std::list<const xBar::LightContainer> m_lights;
      GPUImmutableBuffer m_lightBuffer;

      std::list<const xBar::ShadowLightContainer> m_shadowLights;
      util::SharedPointer<db::Texture2D> m_shadowDepthMap;
      util::SharedPointer<db::Texture3D> m_shadowMaps;
      GPUImmutableBuffer m_shadowedLightBuffer;
      Renderquad m_renderShadowMapsQuad;

      std::list<const xBar::ShadowLightContainer> m_reflectiveShadowLights;
      util::SharedPointer<db::Texture2D> m_reflectiveShadowDepthMap;
      util::SharedPointer<db::Texture3D> m_shadowPosMaps;
      util::SharedPointer<db::Texture3D> m_shadowNormalMaps;
      util::SharedPointer<db::Texture3D> m_shadowLuminousFluxMaps;
      GPUImmutableBuffer m_reflectiveShadowedLightBuffer;
      Renderquad m_renderReflectiveShadowMapsQuad;

      Renderquad m_renderLightMapQuad;
      util::SharedPointer<db::Texture2D> m_lightTexture;

      util::ResourceHandle m_directLightShaderHandle;

      GLint m_maxShadowMapsPerTextureArray;
      GLint m_maxColorAttachments;

      bool m_lightNumberChanged;
      bool m_shadowLightNumberChanged;
      bool m_reflectiveShadowLightNumberChanged;
    };
  }
}

#endif
