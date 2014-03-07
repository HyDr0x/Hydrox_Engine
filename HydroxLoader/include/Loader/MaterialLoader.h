#ifndef MATERIALLOADER_H_
#define MATERIALLOADER_H_

#include "Loader/ResourceLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI MaterialLoader : public ResourceLoader
    {
    public:

      MaterialLoader(util::SingletonManager *singletonManager, unsigned int texNumber);
      ~MaterialLoader();

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource();

    private:

      renderer::MaterialManager *m_materialManager;
      renderer::TextureManager *m_textureManager;
      renderer::RenderShaderManager *m_renderShaderManager;

      const unsigned int m_texNumber;
    };
  }
}

#endif
