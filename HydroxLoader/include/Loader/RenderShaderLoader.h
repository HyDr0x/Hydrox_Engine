#ifndef RENDERSHADERLOADER_H_
#define RENDERSHADERLOADER_H_

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

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource() const;
    };
  }
}

#endif
