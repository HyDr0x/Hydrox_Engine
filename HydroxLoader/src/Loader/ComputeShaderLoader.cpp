#include "Loader/ComputeShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Shader/ShaderContainer.h>

namespace he
{
  namespace loader
  {
    ComputeShaderLoader::ComputeShaderLoader()
    {
    }

    ComputeShaderLoader::~ComputeShaderLoader()
    {
    }

    sh::ComputeShader ComputeShaderLoader::loadResource(std::string shaderName, std::string filename)
    {
      std::string computeShaderSource = loadShaderSource(filename);

      util::ResourceHandle shaderHandle;

      if(computeShaderSource == std::string())
      {
        std::clog << "ERROR, couldn't open file: " << filename << std::endl;
        assert(false);
      }

      return sh::ComputeShader(shaderName, computeShaderSource);
    }

    void ComputeShaderLoader::loadShadersInIndexfile(std::string path, std::string shaderIndexFilename)
    {
      if(shaderIndexFilename == std::string())
      {
        return;
      }

      util::SharedPointer<sh::ShaderContainer> container = m_singletonManager->getService<sh::ShaderContainer>();

      std::ifstream file(path + shaderIndexFilename);
      std::string line;
      unsigned int shaderIndex = UINT32_MAX;

      if(file.is_open())
      {
        while(!file.eof())
        {
          std::getline(file, line, '\n');
          //line += '\n';

          if(line.size() > 0 && line[0] == '#')
          {
            continue;
          }

          size_t pos = line.find("Shader");
          if(pos != std::string::npos)
          {
            shaderIndex = atoi(line.substr(pos + 7).c_str());

            std::vector<std::string> shaderFileNames(2);
            std::getline(file, shaderFileNames[0], '\n');
            std::getline(file, shaderFileNames[1], '\n');
            shaderFileNames[1] = path + shaderFileNames[1];

            container->addComputeShader(shaderIndex, loadResource(shaderFileNames[0], shaderFileNames[1]));

            continue;
          }
        }

        file.close();
      }
      else
      {
        file.close();

        //std::clog << "Warning, couldn't open shader source file: " << filename << std::endl;
      }
    }

    void ComputeShaderLoader::checkIfShaderChanged()
    {
      if(m_shaderFileChecker.isFileChanged())
      {
        m_shaderFileChecker.getChangedFilepath();
      }
    }
  }
}