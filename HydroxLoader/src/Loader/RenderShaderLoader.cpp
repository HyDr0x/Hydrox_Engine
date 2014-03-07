#include "Loader/RenderShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Renderer/Resources/RenderShader.h>

namespace he
{
  namespace loader
  {
    RenderShaderLoader::RenderShaderLoader(util::SingletonManager *singletonManager) : ShaderLoader(singletonManager), m_renderShaderManager(m_singletonManager->getService<renderer::RenderShaderManager>())
    {
      m_fragmentShaderProgram = false;
      m_geometryShaderProgram = false;
      m_tesselationControlShaderProgram = false;
      m_tesselationEvalShaderProgram = false;
    }

    RenderShaderLoader::~RenderShaderLoader()
    {
    }

    void RenderShaderLoader::setUsedShaderPrograms(bool fragmentShaderProgram,
                                                   bool geometryShaderProgram,
                                                   bool tesselationControlShaderProgram,
                                                   bool tesselationEvalShaderProgram)
    {
      m_fragmentShaderProgram = fragmentShaderProgram;
      m_geometryShaderProgram = geometryShaderProgram;
      m_tesselationControlShaderProgram = tesselationControlShaderProgram;
      m_tesselationEvalShaderProgram = tesselationEvalShaderProgram;
    }

    util::ResourceHandle RenderShaderLoader::loadResource(std::string filename)
    {
      std::string vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource;

      vertexShaderSource = loadShaderSource(filename + ".vert");
      if(m_fragmentShaderProgram) fragmentShaderSource = loadShaderSource(filename + ".frag");
      if(m_geometryShaderProgram) geometryShaderSource = loadShaderSource(filename + ".geom");
      if(m_tesselationControlShaderProgram) tesselationCTRLShaderSource = loadShaderSource(filename + ".control");
      if(m_tesselationEvalShaderProgram) tesselationEVALShaderSource = loadShaderSource(filename + ".eval");

      util::ResourceHandle shaderHandle;

      if(vertexShaderSource == std::string() || 
        (m_fragmentShaderProgram && fragmentShaderSource == std::string()) || 
        (m_geometryShaderProgram && geometryShaderSource == std::string()) || 
        (m_tesselationControlShaderProgram && tesselationCTRLShaderSource == std::string()) || 
        (m_tesselationEvalShaderProgram && tesselationEVALShaderSource == std::string()))
      {
        std::cout << "ERROR, couldn't open file: " << filename << std::endl;

        shaderHandle = getDefaultResource();
      }
      else
      {
        shaderHandle = m_renderShaderManager->addObject(renderer::RenderShader(filename, vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource));
      }

      return shaderHandle;
    }

    util::ResourceHandle RenderShaderLoader::getDefaultResource()
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