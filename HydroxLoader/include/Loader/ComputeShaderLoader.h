#ifndef COMPUTESHADERLOADER_H_
#define COMPUTESHADERLOADER_H_

#include "Loader/ShaderLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI ComputeShaderLoader : public ShaderLoader
    {
    public:

      ComputeShaderLoader(renderer::ComputeShaderManager *computeShaderManager);
      ~ComputeShaderLoader();

      util::ResourceHandle loadShader(std::string path, std::string shaderName, std::string computeShaderFilename, std::vector<std::string>& dynamicDefines = std::vector<std::string>());

      util::ResourceHandle getDefaultComputeShader();

    private:

      renderer::ComputeShaderManager *m_computeShaderManager;
    };
  }
}

#endif
