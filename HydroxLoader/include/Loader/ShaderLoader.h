#ifndef SHADERLOADER_H_
#define SHADERLOADER_H_

#include <string>
#include <vector>

#include <Shader/ShaderContainer.h>

#include <Utilities/Miscellaneous/Filecheck.h>
#include <Utilities/Miscellaneous/SingletonBehaviour.hpp>

#include "Loader/DLLExport.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace loader
  {
    class GRAPHICAPI ShaderLoader : public util::SingletonBehaviour
    {
    public:

      ShaderLoader();
      virtual ~ShaderLoader();

      void initialize(util::SingletonManager *singletonManager, std::string shaderDirectory);

      void setDynamicDefines(std::vector<std::pair<std::string, std::string>>& dynamicDefines);

      virtual void checkIfShaderChanged() = 0;

    protected:

      std::string loadShaderSource(std::string filename);
      
      util::SharedPointer<sh::ShaderContainer> m_container;

      util::Filecheck m_shaderFileChecker;

    private:

      std::string loadShaderFileSource(std::string filename);
      void preprocessIncludesShaderSource(std::string& source, std::string path);
      void preprocessDefinesShaderSource(std::string& source);

      std::vector<std::pair<std::string, std::string>> m_dynamicDefines;//first is define name, second define value
    };
  }
}

#endif
