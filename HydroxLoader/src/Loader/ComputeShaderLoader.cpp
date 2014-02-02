#include "Loader/ComputeShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Renderer/Resources/ComputeShader.h>

namespace he
{
  namespace loader
  {
    ComputeShaderLoader::ComputeShaderLoader(renderer::ComputeShaderManager *computeShaderManager) : m_computeShaderManager(computeShaderManager)
    {
    }

    ComputeShaderLoader::~ComputeShaderLoader()
    {
    }

    util::ResourceHandle ComputeShaderLoader::loadShader(std::string path, std::string shaderName, std::string computeShaderFilename, std::vector<std::string>& dynamicDefines)
    {
      std::string computeShaderSource = loadShaderSource(computeShaderFilename, path, dynamicDefines);

      util::ResourceHandle shaderHandle;

      bool noComputeShader = computeShaderSource == std::string();

      if(noComputeShader)
      {
        std::cout << "ERROR, couldn't open file: " << shaderName << std::endl;

        shaderHandle = getDefaultComputeShader();
      }
      else
      {
        shaderHandle = m_computeShaderManager->addObject(renderer::ComputeShader(shaderName, computeShaderSource));
      }

      return shaderHandle;
    }

    util::ResourceHandle ComputeShaderLoader::getDefaultComputeShader()
    {
      std::string computeSource = "#version 430 core\n\
                                  \n\
                                  layout(local_size_x = 1) in;\n\
                                  \n\
                                  void main()\n\
                                  {\n\
                                  }";

      return m_computeShaderManager->addObject(renderer::ComputeShader(std::string("defaultComputeShader"), computeSource));
    }
  }
}