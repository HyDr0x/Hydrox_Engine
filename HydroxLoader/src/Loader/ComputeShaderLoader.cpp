#include "Loader/ComputeShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ComputeShader.h>
#include <DataBase/ShaderContainer.h>

namespace he
{
  namespace loader
  {
    ComputeShaderLoader::ComputeShaderLoader(util::SingletonManager *singletonManager) : ShaderLoader(singletonManager)
    {
    }

    ComputeShaderLoader::~ComputeShaderLoader()
    {
    }

    util::ResourceHandle ComputeShaderLoader::loadResource(std::string shaderName, std::string filename)
    {
      std::string computeShaderSource = loadShaderSource(filename);

      util::ResourceHandle shaderHandle;

      if(computeShaderSource == std::string())
      {
        std::cerr << "ERROR, couldn't open file: " << filename << std::endl;
        assert(false);
      }
      else
      {
        shaderHandle = m_singletonManager->getService<db::ComputeShaderManager>()->addObject(db::ComputeShader(shaderName, computeShaderSource));
      }

      return shaderHandle;
    }

    void ComputeShaderLoader::loadIndexfile(std::string path, std::string shaderIndexFilename)
    {
      if(shaderIndexFilename == std::string())
      {
        return;
      }

      util::SharedPointer<db::ShaderContainer> container = m_singletonManager->getService<db::ShaderContainer>();

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

            container->m_computeShader[shaderIndex] = loadResource(shaderFileNames[0], shaderFileNames[1]);

            continue;
          }
        }

        file.close();
      }
      else
      {
        file.close();

        //std::cerr << "Warning, couldn't open shader source file: " << filename << std::endl;
      }
    }
  }
}