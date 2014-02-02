#ifndef SHADERLOADER_H_
#define SHADERLOADER_H_

#include <string>
#include <vector>

#include <Renderer/Resources/ResourceManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

namespace he
{
  namespace loader
  {
    class GRAPHICAPI ShaderLoader
    {
    public:

      ShaderLoader();
      virtual ~ShaderLoader() = 0;

    protected:

      std::string loadShaderSource(std::string shaderFilename, std::string shaderPath, std::vector<std::string>& dynamicDefines);
    };
  }
}

#endif
