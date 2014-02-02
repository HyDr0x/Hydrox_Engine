#include "Loader/ShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Renderer/Resources/Shader.h>

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

    std::string ShaderLoader::loadShaderSource(std::string shaderFilename, std::string shaderPath, std::vector<std::string>& dynamicDefines)
    {
      if(shaderFilename == std::string())
      {
        return std::string();
      }

      shaderFilename = shaderPath + shaderFilename;

      std::ifstream file(shaderFilename);
      std::string shaderSource;
	    std::string line;

	    if(file.is_open())
      {
		    while(!file.eof())
		    {
			    std::getline(file, line);
			    line += '\n';

          for(int i = 0; i < dynamicDefines.size(); i++)
          {
            size_t pos = line.find(dynamicDefines[i]);
            if(pos != std::string::npos)
            {
              size_t defineOffset = dynamicDefines[i].find(" ");
              line.insert(pos + defineOffset, dynamicDefines[i], defineOffset, std::string::npos);
              dynamicDefines.erase(dynamicDefines.begin() + i);
            }
          }

			    shaderSource += line;
		    }
      }
	    else
	    {
		    file.close();

		    std::cout << "Error: couldn't open shader source file " << shaderFilename << "." << std::endl;

        return std::string();
	    }

	    file.close();

      return shaderSource;
    }
  }
}