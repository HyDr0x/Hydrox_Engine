#ifndef LIGHTRENDERER_H_
#define LIGHTRENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/Pipeline/Renderquad.h"
#include "Renderer/Resources/Texture2D.h"

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
    class Light;

    class LightRenderer
    {
    public:

      LightRenderer();
      ~LightRenderer();

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle directLightShaderHandle);

      void render() const;

      void addRenderComponent(const xBar::LightContainer& light);
      void removeRenderComponent(const xBar::LightContainer& light);

    private:

      LightRenderer(const LightRenderer&);
      LightRenderer& operator=(const LightRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      Renderquad m_renderQuad;
      Texture2D m_lightTexture;

      util::ResourceHandle m_directLightShaderHandle;

      std::list<const xBar::LightContainer> m_lights;

      LightManager *m_lightManager;
    };
  }
}

#endif
