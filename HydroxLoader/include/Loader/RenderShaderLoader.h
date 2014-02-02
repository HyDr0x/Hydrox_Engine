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

      RenderShaderLoader(renderer::RenderShaderManager *renderShaderManager);
      ~RenderShaderLoader();

      util::ResourceHandle loadShader(std::string path, std::string shaderName,
                                std::string vertexShaderFilename, 
			                          std::string fragmentShaderFilename, 
			                          std::string geometryShaderFilename = std::string(), 
			                          std::string tesselationCTRLShaderFilename = std::string(), 
                                std::string tesselationEVALShaderFilename = std::string(),
                                std::vector<std::string>& dynamicDefines = std::vector<std::string>());

      util::ResourceHandle getDefaultRenderShader();

    private:

      renderer::RenderShaderManager *m_renderShaderManager;
    };
  }
}

#endif
