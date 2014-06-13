#ifndef SPRITERENDERER_H_
#define SPRITERENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace util
  {
    class SingletonManager;
    class EventManager;
  }

  namespace db
  {
    class Sprite;
  }

  namespace renderer
  {
    class SpriteRenderer
    {
    public:

      SpriteRenderer();
      ~SpriteRenderer();

      const size_t getMaxSpriteLayer() const;

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle spriteShaderHandle, unsigned char maxLayer);

      void render() const;

      void addRenderComponent(const db::Sprite* sprite);
      void removeRenderComponent(const db::Sprite* sprite);

    private:

      SpriteRenderer(const SpriteRenderer&);
      SpriteRenderer& operator=(const SpriteRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      GLuint m_dummyVBO;

      util::ResourceHandle m_spriteShaderHandle;

      unsigned char m_maxLayer;

      std::list<const db::Sprite*> m_opaqueSprites;
      std::vector<std::list<const db::Sprite*>> m_transparentSprites;

      db::RenderShaderManager *m_renderShaderManager;
      db::TextureManager *m_textureManager;
    };
  }
}

#endif
