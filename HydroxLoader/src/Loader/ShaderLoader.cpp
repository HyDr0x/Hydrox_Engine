#include "Loader/ShaderLoader.h"

#include <fstream>
#include <iostream>

#include <DataBase/Shader.h>

namespace he
{
  namespace loader
  {
    ShaderLoader::ShaderLoader(util::SingletonManager *singletonManager) : ResourceLoader(singletonManager)
    {
    }

    ShaderLoader::~ShaderLoader()
    {
    }

    void ShaderLoader::setDynamicDefines(std::vector<std::string>& dynamicDefines)
    {
      m_dynamicDefines = dynamicDefines;
    }

    std::string ShaderLoader::loadShaderSource(std::string filename)
    {
      if(filename == std::string())
      {
        return std::string();
      }

      std::ifstream file(filename);
      std::string shaderSource;
      std::string line;

      if(file.is_open())
      {
        while(!file.eof())
        {
          std::getline(file, line, '\0');
          //line += '\n';

          shaderSource += line;
        }

        file.close();

        size_t pathEndPos = filename.find_last_of('/') + 1;
        std::string path = filename.substr(0, pathEndPos);
        preprocessShaderSource(shaderSource, path);
      }
      else
      {
        file.close();

        //std::cerr << "Warning, couldn't open shader source file: " << filename << std::endl;
      }

      return shaderSource;
    }

    void ShaderLoader::preprocessShaderSource(std::string& source, std::string path)
    {
      size_t stringPos;
      std::string includeString = "#include";
      do
      {
        stringPos = source.find(includeString);
        
        if(stringPos != std::string::npos)
        {
          size_t filenameStart = source.find('"', stringPos) + 1;
          size_t filenameEnd = source.find('"', filenameStart + 1);
          std::string filename = path + source.substr(filenameStart, filenameEnd - filenameStart);
          std::string includeSource = loadShaderSource(filename);
          source.replace(stringPos, filenameEnd + 2 - stringPos, includeSource, 0, std::string::npos);
        }

      }while(stringPos != std::string::npos);



      for(unsigned int i = 0; i < m_dynamicDefines.size(); i++)
      {
        size_t pos = source.find(m_dynamicDefines[i]);
        if(pos != std::string::npos)
        {
          size_t defineOffset = m_dynamicDefines[i].find(" ");
          source.insert(pos + defineOffset, m_dynamicDefines[i], defineOffset, std::string::npos);
          m_dynamicDefines.erase(m_dynamicDefines.begin() + i);
        }
      }
    }
  }
}