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

      ComputeShaderLoader();
      ~ComputeShaderLoader();

      sh::ComputeShader loadResource(const std::string& shaderName, const std::vector<std::string>& filename);

      void loadShadersInIndexfile(std::string path, std::string shaderIndexFilename);

      virtual void checkIfShaderChanged();

    private:

      std::map<std::string, unsigned int> m_computeShaderMap;
    };
  }
}

#endif
