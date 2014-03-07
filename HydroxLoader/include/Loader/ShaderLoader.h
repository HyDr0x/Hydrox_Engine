#ifndef SHADERLOADER_H_
#define SHADERLOADER_H_

#include "Loader/ResourceLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI ShaderLoader : public ResourceLoader
    {
    public:

      ShaderLoader(util::SingletonManager *singletonManager);
      virtual ~ShaderLoader() = 0;

      void setDynamicDefines(std::vector<std::string>& dynamicDefines);

    protected:

      std::string loadShaderSource(std::string filename);

      std::vector<std::string> m_dynamicDefines;
    };
  }
}

#endif
