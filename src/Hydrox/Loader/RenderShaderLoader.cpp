#include "Hydrox/Loader/RenderShaderLoader.h"

#include <fstream>
#include <iostream>

#include "Hydrox/Graphics/RenderShader.h"

namespace he
{
  RenderShaderLoader::RenderShaderLoader() : m_renderShaderManager(RenderShaderManager::getInstance())
  {
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

      shaderHandle = getDefaultRenderShader();
    }
    else
    {
      shaderHandle = m_renderShaderManager->addObject(RenderShader(shaderName, vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource));
    }

    return shaderHandle;
  }

  ResourceHandle RenderShaderLoader::getDefaultRenderShader()
  {
    std::string vertexSource = "#version 430 core\n\
                                layout(location = 0) uniform mat4 MVP;\n\
                                \n\
                                layout(location = 0) in vec3 in_Pos;\n\
                                \n\
                                void main()\n\
                                {\n\
	                                gl_Position = MVP * vec4(in_Pos, 1);\n\
                                }";

    std::string fragmentSource = "#version 430 core\n\
                                  layout(early_fragment_tests) in;\n\
                                  \n\
                                  out vec4 color;\n\
                                  \n\
                                  void main()\n\
                                  {\n\
	                                  color = vec4(1,1,1,1);\n\
                                  }";

    return RenderShaderManager::getInstance()->addObject(RenderShader(std::string("defaultRenderShader"), vertexSource, fragmentSource));
  }
}