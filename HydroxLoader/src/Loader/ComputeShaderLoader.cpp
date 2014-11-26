#include "Loader/ComputeShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ComputeShader.h>

namespace he
{
  namespace loader
  {
    ComputeShaderLoader::ComputeShaderLoader(util::SingletonManager *singletonManager) : ShaderLoader(singletonManager), ResourceLoader(singletonManager), m_computeShaderManager(singletonManager->getService<db::ComputeShaderManager>())
    {
    }

    ComputeShaderLoader::~ComputeShaderLoader()
    {
    }

    util::ResourceHandle ComputeShaderLoader::loadResource(std::string filename)
    {
      std::string computeShaderSource = loadShaderSource(filename + ".comp");

      util::ResourceHandle shaderHandle;

      if(computeShaderSource == std::string())
      {
        std::cerr << "ERROR, couldn't open file: " << filename << std::endl;

        shaderHandle = getDefaultResource();
      }
      else
      {
        shaderHandle = m_computeShaderManager->addObject(db::ComputeShader(filename, computeShaderSource));
      }

      return shaderHandle;
    }

    util::ResourceHandle ComputeShaderLoader::getDefaultResource() const
    {
      std::string computeSource = "#version 430 core\n\
                                  \n\
                                  layout(local_size_x = 1) in;\n\
                                  /n\
                                  void main()\n\
                                  {\n\
                                  }";

      return m_computeShaderManager->addObject(db::ComputeShader(std::string("defaultComputeShader"), computeSource));
    }
  }
}