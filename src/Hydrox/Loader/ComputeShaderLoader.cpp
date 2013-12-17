#include "Hydrox/Loader/ComputeShaderLoader.h"

#include <fstream>
#include <iostream>

#include "Hydrox/Graphics/ComputeShader.h"

namespace he
{
  ComputeShaderLoader::ComputeShaderLoader(ComputeShaderManager *computeShaderManager)
  {
    m_computeShaderManager = computeShaderManager;
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
      shaderHandle = m_computeShaderManager->getDefaultResource();
    }
    else
    {
      shaderHandle = m_computeShaderManager->addObject(ComputeShader(shaderName, computeShaderSource));
    }

    return shaderHandle;
  }
}