#ifndef LIGHTRENDERER_H_
#define LIGHTRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/Texture2D.h>

#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Pipeline/Renderquad.h"
#include "Renderer/Pipeline/RenderOptions.h"

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
    class LightRenderer
    {
    public:

      LightRenderer();
      ~LightRenderer();

      void initialize(RenderOptions options, util::SingletonManager *singletonManager, util::ResourceHandle directLightShaderHandle);

      void updateBuffer();
      void render(util::SharedPointer<db::Texture2D> depthMap, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap);

      util::SharedPointer<db::Texture2D> getLightTexture() const;

      void setShadowMap(unsigned int bindingPoint, unsigned int shadowMapIndex);
      void unsetShadowMap(unsigned int bindingPoint);

      void addLight(const xBar::LightContainer& light);
      void removeLight(const xBar::LightContainer& light);

      void addShadowLight(const xBar::ShadowLightContainer& light);
      void removeShadowLight(const xBar::ShadowLightContainer& light);

      void clear() const;

      unsigned int getShadowLightNumber() const;

      util::SharedPointer<db::Texture3D> getShadowMaps() const;

    private:
      
      LightRenderer(const LightRenderer&);
      LightRenderer& operator=(const LightRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      RenderOptions m_options;

      util::SingletonManager *m_singletonManager;

      std::list<const xBar::ShadowLightContainer> m_shadowLights;
      GPUImmutableBuffer m_shadowedLightBuffer;
      util::SharedPointer<db::Texture3D> m_shadowMaps;
      Renderquad m_renderShadowMapsQuad;

      std::list<const xBar::LightContainer> m_lights;
      GPUImmutableBuffer m_lightBuffer;

      Renderquad m_renderLightMapQuad;
      util::SharedPointer<db::Texture2D> m_lightTexture;

      util::ResourceHandle m_createShadowMapShaderHandle;

      util::ResourceHandle m_directLightShaderHandle;
      util::ResourceHandle m_shadowMapShaderHandle;
      util::ResourceHandle m_cascadedShadowMapShaderHandle;

      GLint m_maxShadowMapsPerTextureArray;

      bool m_lightNumberChanged;
      bool m_shadowLightNumberChanged;
    };
  }
}

#endif
