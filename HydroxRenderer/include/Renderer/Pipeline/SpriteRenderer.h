#ifndef SPRITERENDERER_H_
#define SPRITERENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include <Renderer/Resources/ResourceManager.hpp>

#include "Renderer/Buffer/GPUBuffer.h"

#include "Renderer/Resources/RenderShader.h"
#include "Renderer/Resources/Texture.h"
#include "Renderer/Resources/Sprite.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

namespace he
{
	namespace renderer
	{
    class SpriteRenderer
    {
    public:

      SpriteRenderer();
	    ~SpriteRenderer();

      const size_t getMaxSpriteLayer() const;

      void initialize(util::SingletonManager *singletonManager, size_t maxSpriteLayer, util::ResourceHandle spriteShaderHandle);

	    void render();

      void addRenderComponent(Sprite* sprite);
      void removeRenderComponent(Sprite* sprite);

    private:

	    SpriteRenderer(const SpriteRenderer&){}

      void registerRenderComponentSlots(util::EventManager *eventManager);

      GLuint m_dummyVBO;

      util::ResourceHandle m_spriteShaderHandle;

      size_t m_maxLayer;
      std::list<Sprite*> m_opaqueSprites;
      std::vector<std::list<Sprite*>> m_transparentSprites;

	    RenderShaderManager *m_renderShaderManager;
      TextureManager *m_textureManager;
    };
	}
}

#endif
