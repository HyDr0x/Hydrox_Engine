#ifndef SPRITERENDERER_H_
#define SPRITERENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include <DataBase/ResourceManager.hpp>

#include <Shader/ShaderContainer.h>

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

      void initialize(util::SingletonManager *singletonManager);

      void render() const;

      void addRenderComponent(const db::Sprite* sprite);
      void removeRenderComponent(const db::Sprite* sprite);

    private:

      SpriteRenderer(const SpriteRenderer&);
      SpriteRenderer& operator=(const SpriteRenderer&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      sh::RenderShaderHandle m_spriteShaderHandle;

      GLuint m_dummyVBO;

      unsigned char m_maxLayer;

      std::vector<std::list<const db::Sprite*>> m_opaqueSprites;
      std::vector<std::list<const db::Sprite*>> m_transparentSprites;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;
      util::SharedPointer<db::TextureManager> m_textureManager;
    };
  }
}

#endif
