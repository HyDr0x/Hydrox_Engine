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

            registerShaderSourceFiles(shaderFileNames[0], shaderIndex);
            //m_computeShaderMap[shaderFileNames[0]].push_back(shaderIndex);
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
        for(unsigned int i = 0; i < m_computeShaderMap[m_shaderFileChecker.getChangedFilepath()].size(); i++)
        {
          const sh::ComputeShader& shader = m_container->getComputeShader(m_computeShaderMap[m_shaderFileChecker.getChangedFilepath()][i]);

          m_container->replaceComputeShader(m_computeShaderMap[m_shaderFileChecker.getChangedFilepath()][i], loadResource(shader.getShaderName(), shader.getShaderSourceNames()));
        }

        m_shaderFileChecker.resetFilecheckerStatus();
      }
    }

    void ComputeShaderLoader::registerShaderSourceFiles(const std::string& shaderFilename, unsigned int shaderHandle)
    {
      bool alreadyIn = false;
      for(unsigned int i = 0; i < m_computeShaderMap[shaderFilename].size(); i++)
      {
        if(m_computeShaderMap[shaderFilename][i] == shaderHandle)
        {
          alreadyIn = true;
          break;
        }
      }

      if(!alreadyIn)//only insert it if it's not already in there
      {
        m_computeShaderMap[shaderFilename].push_back(shaderHandle);
      }

      std::string shaderSource = loadFile(shaderFilename);

      size_t stringPos = 0;
      std::string includeString = "#include";

      do
      {
        stringPos = shaderSource.find(includeString, stringPos);

        if(stringPos != std::string::npos)
        {
          size_t filenameStart = shaderSource.find('"', stringPos) + 1;
          size_t filenameEnd = shaderSource.find('"', filenameStart + 1);
          std::string filename = shaderSource.substr(filenameStart, filenameEnd - filenameStart);

          registerShaderSourceFiles(filename, shaderHandle);

          stringPos++;//to not find the same include again
        }

      } while(stringPos != std::string::npos);
    }
  }
}