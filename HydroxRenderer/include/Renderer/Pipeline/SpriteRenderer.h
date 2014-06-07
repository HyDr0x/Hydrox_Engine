#ifndef SPRITERENDERER_H_
#define SPRITERENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include "Renderer/Resources/ResourceManager.hpp"

namespace he
{
  namespace util
  {
    class SingletonManager;
    class EventManager;
  }

  namespace renderer
  {
    class Sprite;

    class SpriteRenderer
    {
    public:

      SpriteRenderer();
      ~SpriteRenderer();

      const size_t getMaxSpriteLayer() const;

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle spriteShaderHandle, unsigned char maxLayer);

      void render() const;

      void addRenderComponent(const Sprite* sprite);
      void removeRenderComponent(const Sprite* sprite);

    private:

      SpriteRenderer(const SpriteRenderer&);
      SpriteRenderer& operator=(const SpriteRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      GLuint m_dummyVBO;

      util::ResourceHandle m_spriteShaderHandle;

      unsigned char m_maxLayer;

      std::list<const Sprite*> m_opaqueSprites;
      std::vector<std::list<const Sprite*>> m_transparentSprites;

      RenderShaderManager *m_renderShaderManager;
      TextureManager *m_textureManager;
    };
  }
}

#endif
