#ifndef RENDERSHADERLOADER_H_
#define RENDERSHADERLOADER_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Loader/ShaderLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI RenderShaderLoader : public ShaderLoader
    {
    public:

      RenderShaderLoader(util::SingletonManager *singletonManager);
      ~RenderShaderLoader();

      util::ResourceHandle loadResource(std::string filename,
                                        std::string vertexFilename, 
                                        std::string fragmentFilename = std::string(),
                                        std::string geometryFilename = std::string(),
                                        std::string tessControlFilename = std::string(),
                                        std::string tessEvalFilename = std::string());

      util::ResourceHandle getDefaultResource() const;

      util::ResourceHandle getDefaultShadowResource() const;
    };
  }
}

#endif
