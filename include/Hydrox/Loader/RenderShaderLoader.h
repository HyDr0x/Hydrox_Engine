#ifndef RENDERSHADERLOADER_H_
#define RENDERSHADERLOADER_H_

#include "Hydrox/Loader/ShaderLoader.h"

namespace he
{
  class GRAPHICAPI RenderShaderLoader : public ShaderLoader
  {
  public:

    RenderShaderLoader();
    ~RenderShaderLoader();

    ResourceHandle loadShader(std::string shaderName,
                              std::string vertexShaderFilename, 
			                        std::string fragmentShaderFilename, 
			                        std::string geometryShaderFilename = std::string(), 
			                        std::string tesselationCTRLShaderFilename = std::string(), 
                              std::string tesselationEVALShaderFilename = std::string(),
                              std::vector<std::string>& dynamicDefines = std::vector<std::string>());

  private:

    RenderShaderManager *m_renderShaderManager;
  };
}

#endif
