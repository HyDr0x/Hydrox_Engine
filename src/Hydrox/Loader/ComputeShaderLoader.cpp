#include "Hydrox/Loader/ComputeShaderLoader.h"

#include <fstream>
#include <iostream>

#include "Hydrox/Graphics/ComputeShader.h"

namespace he
{
  ComputeShaderLoader::ComputeShaderLoader() : m_computeShaderManager(ComputeShaderManager::getInstance())
  {
  }

  ComputeShaderLoader::~ComputeShaderLoader()
  {
  }

  ResourceHandle ComputeShaderLoader::loadShader(std::string shaderName, std::string computeShaderFilename, std::vector<std::string>& dynamicDefines)
  {
    std::string computeShaderSource = loadShaderSource(computeShaderFilename, m_computeShaderManager->getPath(), dynamicDefines);

    ResourceHandle shaderHandle;

    bool noComputeShader = computeShaderSource == std::string();

    if(noComputeShader)
    {
      std::cout << "ERROR, couldn't open file: " << shaderName << std::endl;

      shaderHandle = getDefaultComputeShader();
    }
    else
    {
      shaderHandle = m_computeShaderManager->addObject(ComputeShader(shaderName, computeShaderSource));
    }

    return shaderHandle;
  }

  ResourceHandle ComputeShaderLoader::getDefaultComputeShader()
  {
    std::string computeSource = "#version 430 core\n\
                                \n\
                                layout(local_size_x = 1) in;\n\
                                \n\
                                void main()\n\
                                {\n\
                                }";

    return ComputeShaderManager::getInstance()->addObject(ComputeShader(std::string("defaultComputeShader"), computeSource));
  }
}