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

      ComputeShaderLoader(util::SingletonManager *singletonManager);
      ~ComputeShaderLoader();

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource();

    private:

      renderer::ComputeShaderManager *m_computeShaderManager;
    };
  }
}

#endif
