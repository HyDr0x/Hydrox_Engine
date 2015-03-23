#ifndef SHADERLOADER_H_
#define SHADERLOADER_H_

#include <string>
#include <vector>

#include "Loader/DLLExport.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace loader
  {
    class GRAPHICAPI ShaderLoader
    {
    public:

      ShaderLoader(util::SingletonManager *singletonManager);
      virtual ~ShaderLoader();

      void setDynamicDefines(std::vector<std::pair<std::string, std::string>>& dynamicDefines);

    protected:

      std::string loadShaderSource(std::string filename);
      
      util::SingletonManager *m_singletonManager;

    private:

      std::string loadShaderFileSource(std::string filename);
      void preprocessIncludesShaderSource(std::string& source, std::string path);
      void preprocessDefinesShaderSource(std::string& source);

      std::vector<std::pair<std::string, std::string>> m_dynamicDefines;//first is define name, second define value
    };
  }
}

#endif
