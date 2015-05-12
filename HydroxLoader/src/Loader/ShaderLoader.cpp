#include "Loader/ShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Shader/Shader.h>

namespace he
{
  namespace loader
  {
    ShaderLoader::ShaderLoader()
    {
    }

    ShaderLoader::~ShaderLoader()
    {
    }

    void ShaderLoader::initialize(util::SingletonManager *singletonManager, std::string shaderDirectory)
    {
      m_singletonManager = singletonManager;

      m_shaderFileChecker.initialize(shaderDirectory);
    }

    void ShaderLoader::setDynamicDefines(std::vector<std::pair<std::string, std::string>>& dynamicDefines)
    {
      m_dynamicDefines = dynamicDefines;
    }

    std::string ShaderLoader::loadShaderSource(std::string filename)
    {
      std::string rawSource = loadShaderFileSource(filename);

      preprocessDefinesShaderSource(rawSource);

      return rawSource;
    }

    std::string ShaderLoader::loadShaderFileSource(std::string filename)
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

        preprocessIncludesShaderSource(shaderSource, path);
      }
      else
      {
        file.close();

        //std::cerr << "Warning, couldn't open shader source file: " << filename << std::endl;
      }

      return shaderSource;
    }

    void ShaderLoader::preprocessIncludesShaderSource(std::string& source, std::string path)
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
          std::string includeSource = loadShaderFileSource(filename);
          source.replace(stringPos, filenameEnd + 2 - stringPos, includeSource, 0, std::string::npos);
        }

      }while(stringPos != std::string::npos);
    }

    void ShaderLoader::preprocessDefinesShaderSource(std::string& source)
    {
      size_t stringPos;
      std::string defineString = "#define ";

      for(unsigned int i = 0; i < m_dynamicDefines.size(); i++)//copy paste dynamic defines
      {
        stringPos = source.find(defineString + m_dynamicDefines[i].first);
        if(stringPos != std::string::npos)
        {
          size_t defineOffset = defineString.length() +  m_dynamicDefines[i].first.length();
          source.insert(stringPos + defineOffset, std::string(" ") + m_dynamicDefines[i].second);
        }
      }

      do//replace define names with numbers
      {
        stringPos = source.find(defineString);

        if(stringPos != std::string::npos)
        {
          size_t defineNameEndPos = source.find(" ", stringPos + 8);
          size_t defineNameLength = defineNameEndPos - (stringPos + 8);
          std::string defineName = source.substr(stringPos + 8, defineNameLength);

          size_t defineValueEndPos = source.find("\n", defineNameEndPos + 1);
          size_t defineValueLength = defineValueEndPos - (defineNameEndPos + 1);
          std::string defineValue = source.substr(defineNameEndPos + 1, defineValueLength);

          source.erase(stringPos, defineValueEndPos - stringPos);

          size_t stringPos2;
          do
          {
            stringPos2 = source.find(defineName);

            if(stringPos2 != std::string::npos)
            {
              source.replace(stringPos2, defineNameLength, defineValue, 0, defineValueLength);
            }
          } while(stringPos2 != std::string::npos);
        }

      } while(stringPos != std::string::npos);
    }
  }
}