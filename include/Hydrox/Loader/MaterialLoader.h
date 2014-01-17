#ifndef MATERIALLOADER_H_
#define MATERIALLOADER_H_

#include <string>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Singletons/CacheManager.hpp"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

namespace he
{
  class GRAPHICAPI MaterialLoader
  {
  public:

    MaterialLoader();
    ~MaterialLoader();

    ResourceHandle loadMaterial(std::string materialFilename);

    static ResourceHandle getDefaultMaterial();

  private:

    MaterialManager *m_materialManager;
    TextureManager *m_textureManager;
    RenderShaderManager *m_renderShaderManager;

    static const unsigned int m_TEXNUMBER = 4;
  };
}

#endif
