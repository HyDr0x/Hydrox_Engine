#include "Loader/ShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Renderer/Resources/Shader.h>

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
			    std::getline(file, line);
			    line += '\n';

          for(int i = 0; i < m_dynamicDefines.size(); i++)
          {
            size_t pos = line.find(m_dynamicDefines[i]);
            if(pos != std::string::npos)
            {
              size_t defineOffset = m_dynamicDefines[i].find(" ");
              line.insert(pos + defineOffset, m_dynamicDefines[i], defineOffset, std::string::npos);
              m_dynamicDefines.erase(m_dynamicDefines.begin() + i);
            }
          }

			    shaderSource += line;
		    }
      }
	    else
	    {
		    file.close();

        return std::string();
	    }

	    file.close();

      return shaderSource;
    }
  }
}