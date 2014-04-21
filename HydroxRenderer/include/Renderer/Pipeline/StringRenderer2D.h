#ifndef STRINGRENDERER2D_H_
#define STRINGRENDERER2D_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/Buffer/GPUBuffer.h"

#include "Renderer/Resources/RenderShader.h"
#include "Renderer/Resources/Texture2D.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

namespace he
{
	namespace renderer
	{
    class StringTexture2D;

    class StringRenderer2D
    {
    public:

      StringRenderer2D();
      ~StringRenderer2D();

      const size_t getMaxSpriteLayer() const;

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle stringShaderHandle, unsigned char maxLayer);

	    void render();

      void addRenderComponent(StringTexture2D* sprite);
      void removeRenderComponent(StringTexture2D* sprite);

    private:

      StringRenderer2D(const StringRenderer2D&){}

      void registerRenderComponentSlots(util::EventManager *eventManager);

      util::ResourceHandle m_stringShaderHandle;

      GLuint m_stringVAO;

      unsigned char m_maxLayer;

      std::list<StringTexture2D*> m_opaqueStrings;
      std::vector<std::list<StringTexture2D*>> m_transparentStrings;

	    RenderShaderManager *m_renderShaderManager;
      TextureManager *m_textureManager;
    };
	}
}

#endif
