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

    sh::ComputeShader ComputeShaderLoader::loadResource(const std::string& shaderName, const std::vector<std::string>& filename)
    {
      std::vector<std::string> computeShaderSource(1);
      computeShaderSource[0] = loadShaderSource(filename[0]);

      util::ResourceHandle shaderHandle;

      return sh::ComputeShader(shaderName, filename, computeShaderSource);
    }

    void ComputeShaderLoader::loadShadersInIndexfile(std::string path, std::string shaderIndexFilename)
    {
      if(shaderIndexFilename == std::string())
      {
        return;
      }

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

            std::string shaderName;
            std::getline(file, shaderName, '\n');

            std::vector<std::string> shaderFileNames(1);
            std::getline(file, shaderFileNames[0], '\n');
            shaderFileNames[0] = path + shaderFileNames[0];

            m_container->addComputeShader(shaderIndex, loadResource(shaderName, shaderFileNames));

            m_computeShaderMap[shaderFileNames[0]] = shaderIndex;
          }
        }

        file.close();
      }
      else
      {
        file.close();

        std::clog << "Warning, couldn't open shader source file: " << path + shaderIndexFilename << std::endl;
      }
    }

    void ComputeShaderLoader::checkIfShaderChanged()
    {
      if(m_shaderFileChecker.isFileChanged())
      {
        const sh::ComputeShader& shader = m_container->getComputeShader(m_computeShaderMap[m_shaderFileChecker.getChangedFilepath()]);

        m_container->replaceComputeShader(m_computeShaderMap[m_shaderFileChecker.getChangedFilepath()], loadResource(shader.getShaderName(), shader.getShaderSourceNames()));

        m_shaderFileChecker.resetFilecheckerStatus();
      }
    }
  }
}