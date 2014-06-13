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
  }

  namespace renderer
  {
    class LightRenderer
    {
    public:

      LightRenderer();
      ~LightRenderer();

      void initialize(RenderOptions options, util::SingletonManager *singletonManager, util::ResourceHandle directLightShaderHandle);

      void render(RenderOptions options, util::SharedPointer<db::Texture2D> depthMap, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap);

      util::SharedPointer<db::Texture2D> getLightTexture() const;

      void addRenderComponent(const xBar::LightContainer& light);
      void removeRenderComponent(const xBar::LightContainer& light);

      void clear() const;

    private:
      
      LightRenderer(const LightRenderer&);
      LightRenderer& operator=(const LightRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      util::SingletonManager *m_singletonManager;

      std::list<const xBar::LightContainer> m_lights;

      GPUImmutableBuffer m_lightBuffer;
      Renderquad m_renderQuad;
      util::SharedPointer<db::Texture2D> m_lightTexture;

      util::ResourceHandle m_directLightShaderHandle;

      bool m_lightNumberChanged;
    };
  }
}

#endif
