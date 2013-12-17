#ifndef COMPUTESHADERLOADER_H_
#define COMPUTESHADERLOADER_H_

#include "Hydrox/Loader/ShaderLoader.h"

namespace he
{
  class GRAPHICAPI ComputeShaderLoader : public ShaderLoader
  {
  public:

    ComputeShaderLoader(ComputeShaderManager *computeShaderManager);
    ~ComputeShaderLoader();

    ResourceHandle loadShader(std::string shaderName, std::string computeShaderFilename, std::vector<std::string>& dynamicDefines = std::vector<std::string>());

  private:

    ComputeShaderManager *m_computeShaderManager;
  };
}

#endif