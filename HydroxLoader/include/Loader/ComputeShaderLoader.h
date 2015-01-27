#ifndef COMPUTESHADERLOADER_H_
#define COMPUTESHADERLOADER_H_

#include <DataBase/ResourceManager.hpp>

#include "Loader/ShaderLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI ComputeShaderLoader : public ShaderLoader
    {
    public:

      ComputeShaderLoader(util::SingletonManager *singletonManager);
      ~ComputeShaderLoader();

      util::ResourceHandle loadResource(std::string shaderName, std::string filename);

      void loadIndexfile(std::string path, std::string shaderIndexFilename);
    };
  }
}

#endif
