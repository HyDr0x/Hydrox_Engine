#ifndef SKYBOXRENDERER_H_
#define SKYBOXRENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/Buffer/GPUBuffer.h"

#include "Renderer/Resources/RenderShader.h"
#include "Renderer/Resources/Texture2D.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

namespace he
{
	namespace renderer
	{
    class SkyboxRenderer
    {
    public:

      SkyboxRenderer();
      ~SkyboxRenderer();

      const size_t getMaxSpriteLayer() const;

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle skyboxShaderHandle, util::ResourceHandle skyboxTextureHandles[6]);

	    void render();

    private:

      SkyboxRenderer(const SkyboxRenderer&){}

      util::ResourceHandle m_skyboxShaderHandle;
      util::ResourceHandle m_arrayTextureHandle;

      GLuint m_skyBoxVAO;
      GPUBuffer m_geometryData;

	    RenderShaderManager *m_renderShaderManager;
      TextureManager *m_textureManager;
      TextureArrayManager*m_textureArrayManager;
    };
	}
}

#endif
