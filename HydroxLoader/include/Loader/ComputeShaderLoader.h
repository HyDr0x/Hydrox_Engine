#ifndef COMPUTESHADERLOADER_H_
#define COMPUTESHADERLOADER_H_

#include "Loader/ShaderLoader.h"
#include "Loader/ResourceLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI ComputeShaderLoader : public ShaderLoader, public ResourceLoader
    {
    public:

      ComputeShaderLoader(util::SingletonManager *singletonManager);
      ~ComputeShaderLoader();

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource() const;

    private:

      db::ComputeShaderManager *m_computeShaderManager;
    };
  }
}

#endif
