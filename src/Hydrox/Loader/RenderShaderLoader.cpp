#include "Hydrox/Loader/RenderShaderLoader.h"

#include <fstream>
#include <iostream>

#include "Hydrox/Graphics/RenderShader.h"

namespace he
{
  RenderShaderLoader::RenderShaderLoader(RenderShaderManager *renderShaderManager)
  {
    m_renderShaderManager = renderShaderManager;
  }

  RenderShaderLoader::~RenderShaderLoader()
  {
  }

  ResourceHandle RenderShaderLoader::loadShader(std::string shaderName,
                                                std::string vertexShaderFilename, 
			                                          std::string fragmentShaderFilename, 
			                                          std::string geometryShaderFilename, 
			                                          std::string tesselationCTRLShaderFilename, 
                                                std::string tesselationEVALShaderFilename,
                                                std::vector<std::string>& dynamicDefines)
  {
    std::string shaderPath = m_renderShaderManager->getPath();

    std::string vertexShaderSource = loadShaderSource(vertexShaderFilename, shaderPath, dynamicDefines);
    std::string fragmentShaderSource = loadShaderSource(fragmentShaderFilename, shaderPath, dynamicDefines);
    std::string geometryShaderSource = loadShaderSource(geometryShaderFilename, shaderPath, dynamicDefines);
    std::string tesselationCTRLShaderSource = loadShaderSource(tesselationCTRLShaderFilename, shaderPath, dynamicDefines);
    std::string tesselationEVALShaderSource = loadShaderSource(tesselationEVALShaderFilename, shaderPath, dynamicDefines);

    bool noRenderShader = (
                            vertexShaderSource == std::string() || 
                            (fragmentShaderSource != std::string() && fragmentShaderSource == std::string()) || 
                            (geometryShaderFilename != std::string() && geometryShaderSource == std::string()) ||
                            (tesselationCTRLShaderFilename != std::string() && tesselationCTRLShaderSource == std::string()) ||
                            (tesselationEVALShaderFilename != std::string() && tesselationEVALShaderFilename == std::string())
                          );

    ResourceHandle shaderHandle;

    if(noRenderShader)
    {
      std::cout << "ERROR, couldn't open file: " << shaderName << std::endl;
      shaderHandle = m_renderShaderManager->getDefaultResource();
    }
    else
    {
      shaderHandle = m_renderShaderManager->addObject(RenderShader(shaderName, vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource));
    }

    return shaderHandle;
  }
}