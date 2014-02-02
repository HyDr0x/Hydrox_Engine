#ifndef MATERIALLOADER_H_
#define MATERIALLOADER_H_

#include <string>

#include "Loader/DLLExport.h"

#include <Renderer/Resources/ResourceManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

namespace he
{
  namespace loader
  {
    class GRAPHICAPI MaterialLoader
    {
    public:

      MaterialLoader(renderer::MaterialManager *materialManager, renderer::TextureManager *textureManager, renderer::RenderShaderManager *renderShaderManager);
      ~MaterialLoader();

      util::ResourceHandle loadMaterial(std::string materialPath, std::string texturePath, std::string shaderPath, std::string materialFilename);

      util::ResourceHandle getDefaultMaterial();

    private:

      renderer::MaterialManager *m_materialManager;
      renderer::TextureManager *m_textureManager;
      renderer::RenderShaderManager *m_renderShaderManager;

      static const unsigned int m_TEXNUMBER = 4;
    };
  }
}

#endif
