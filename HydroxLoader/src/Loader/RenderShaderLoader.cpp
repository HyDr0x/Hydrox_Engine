#include "Loader/RenderShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Renderer/Resources/RenderShader.h>

namespace he
{
  namespace loader
  {
    RenderShaderLoader::RenderShaderLoader(util::SingletonManager *singletonManager) : ShaderLoader(singletonManager), m_renderShaderManager(m_singletonManager->getService<renderer::RenderShaderManager>())
    {
    }

    RenderShaderLoader::~RenderShaderLoader()
    {
    }

    util::ResourceHandle RenderShaderLoader::loadResource(std::string filename)
    {
      std::string vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource;

      vertexShaderSource = loadShaderSource(filename + ".vert");
      fragmentShaderSource = loadShaderSource(filename + ".frag");
      geometryShaderSource = loadShaderSource(filename + ".geom");
      tesselationCTRLShaderSource = loadShaderSource(filename + ".control");
      tesselationEVALShaderSource = loadShaderSource(filename + ".eval");

      util::ResourceHandle shaderHandle;

      if(vertexShaderSource.empty())
      {
        std::cerr << "ERROR, couldn't open file: " << filename << std::endl;

        shaderHandle = getDefaultResource();
      }
      else
      {
        shaderHandle = m_renderShaderManager->addObject(renderer::RenderShader(filename, vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource));
      }

      return shaderHandle;
    }

    util::ResourceHandle RenderShaderLoader::getDefaultResource() const
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