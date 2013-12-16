#ifndef SHADERLOADER_H_
#define SHADERLOADER_H_

#include <string>
#include <vector>

#include "Hydrox/Singletons/CacheManager.hpp"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

namespace he
{
  class ShaderLoader
  {
  public:

    ShaderLoader(ShaderManager *shaderManager);
    ~ShaderLoader();

    ResourceHandle loadShader(std::string shaderName,
                              std::string vertexShaderFilename, 
			                        std::string fragmentShaderFilename, 
			                        std::string geometryShaderFilename = std::string(), 
			                        std::string tesselationCTRLShaderFilename = std::string(), 
                              std::string tesselationEVALShaderFilename = std::string(),
                              std::string computeShaderFilename = std::string(),
                              std::vector<std::string>& dynamicDefines = std::vector<std::string>());

  private:

    std::string loadShaderSource(std::string shaderFilename, std::vector<std::string>& dynamicDefines);

    ShaderManager *m_shaderManager;
  };
}

#endif