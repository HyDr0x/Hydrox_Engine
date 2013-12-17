#ifndef SHADERLOADER_H_
#define SHADERLOADER_H_

#include <string>
#include <vector>

#include "Hydrox/Singletons/CacheManager.hpp"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

namespace he
{
  class GRAPHICAPI ShaderLoader
  {
  public:

    ShaderLoader();
    virtual ~ShaderLoader() = 0;

  protected:

    std::string loadShaderSource(std::string shaderFilename, std::string shaderPath, std::vector<std::string>& dynamicDefines);
  };
}

#endif