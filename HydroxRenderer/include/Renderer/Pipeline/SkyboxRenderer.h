#ifndef SKYBOXRENDERER_H_
#define SKYBOXRENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Buffer/GPUBuffer.h"

#include <DataBase/RenderShader.h>
#include <DataBase/Texture2D.h>

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

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle skyboxTextureHandles[6]);

      void render() const;

    private:

      SkyboxRenderer(const SkyboxRenderer&);
      SkyboxRenderer& operator=(const SkyboxRenderer&);

      util::ResourceHandle m_skyboxShaderHandle;
      util::ResourceHandle m_arrayTextureHandle;

      GLuint m_skyBoxVAO;
      GPUBuffer m_geometryData;

      db::RenderShaderManager *m_renderShaderManager;
      db::TextureManager *m_textureManager;
      db::TextureArrayManager*m_textureArrayManager;
    };
  }
}

#endif
