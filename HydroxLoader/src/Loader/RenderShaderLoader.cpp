#include "Loader/RenderShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Renderer/Resources/RenderShader.h>

namespace he
{
  namespace loader
  {
    RenderShaderLoader::RenderShaderLoader(renderer::RenderShaderManager *renderShaderManager) : m_renderShaderManager(renderShaderManager)
    {
    }

    RenderShaderLoader::~RenderShaderLoader()
    {
    }

    util::ResourceHandle RenderShaderLoader::loadShader(std::string path, std::string shaderName,
                                                  std::string vertexShaderFilename, 
			                                            std::string fragmentShaderFilename, 
			                                            std::string geometryShaderFilename, 
			                                            std::string tesselationCTRLShaderFilename, 
                                                  std::string tesselationEVALShaderFilename,
                                                  std::vector<std::string>& dynamicDefines)
    {
      std::string vertexShaderSource = loadShaderSource(vertexShaderFilename, path, dynamicDefines);
      std::string fragmentShaderSource = loadShaderSource(fragmentShaderFilename, path, dynamicDefines);
      std::string geometryShaderSource = loadShaderSource(geometryShaderFilename, path, dynamicDefines);
      std::string tesselationCTRLShaderSource = loadShaderSource(tesselationCTRLShaderFilename, path, dynamicDefines);
      std::string tesselationEVALShaderSource = loadShaderSource(tesselationEVALShaderFilename, path, dynamicDefines);

      bool noRenderShader = (
                              vertexShaderSource == std::string() || 
                              (fragmentShaderSource != std::string() && fragmentShaderSource == std::string()) || 
                              (geometryShaderFilename != std::string() && geometryShaderSource == std::string()) ||
                              (tesselationCTRLShaderFilename != std::string() && tesselationCTRLShaderSource == std::string()) ||
                              (tesselationEVALShaderFilename != std::string() && tesselationEVALShaderFilename == std::string())
                            );

      util::ResourceHandle shaderHandle;

      if(noRenderShader)
      {
        std::cout << "ERROR, couldn't open file: " << shaderName << std::endl;

        shaderHandle = getDefaultRenderShader();
      }
      else
      {
        shaderHandle = m_renderShaderManager->addObject(renderer::RenderShader(shaderName, vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource));
      }

      return shaderHandle;
    }

    util::ResourceHandle RenderShaderLoader::getDefaultRenderShader()
    {
      std::string vertexSource = "#version 440 core\n\
                                  #extension ARB_shader_draw_parameters : enable\n\
                                  \n\
                                  layout(std140, binding = 0) uniform cameraParameters\n\
                                  {\n\
	                                  mat4 viewMatrix;\n\
	                                  mat4 projectionMatrix;\n\
	                                  mat4 viewProjectionMatrix;\n\
	                                  vec4 eyePos;\n\
                                  };\n\
                                  \n\
                                  layout(std430, binding = 0) buffer transformMatrixBuffer\n\
                                  {\n\
	                                  mat4 trfMatrix[];\n\
                                  };\n\
                                  \n\
                                  layout(location = 0) in vec3 in_Pos;\n\
                                  \n\
                                  void main()\n\
                                  {\n\
	                                  gl_Position = viewProjectionMatrix * trfMatrix[gl_InstanceID + gl_BaseInstanceARB] * vec4(in_Pos, 1);\n\
                                  }";

      std::string fragmentSource = "#version 440 core\n\
                                    layout(early_fragment_tests) in;\n\
                                    \n\
                                    out vec4 color;\n\
                                    \n\
                                    void main()\n\
                                    {\n\
	                                    color = vec4(1,1,1,1);\n\
                                    }";

      return m_renderShaderManager->addObject(renderer::RenderShader(std::string("defaultRenderShader"), vertexSource, fragmentSource));
    }
  }
}