#include "Hydrox/Loader/ShaderLoader.h"

#include <fstream>
#include <iostream>

#include "Hydrox/Graphics/Shader.h"

namespace he
{
  ShaderLoader::ShaderLoader(ShaderManager *shaderManager)
  {
    m_shaderManager = shaderManager;
  }

  ShaderLoader::~ShaderLoader()
  {
  }

  ResourceHandle ShaderLoader::loadShader(std::string shaderName,
                                          std::string vertexShaderFilename, 
			                                    std::string fragmentShaderFilename, 
			                                    std::string geometryShaderFilename, 
			                                    std::string tesselationCTRLShaderFilename, 
                                          std::string tesselationEVALShaderFilename,
                                          std::string computeShaderFilename,
                                          std::vector<std::string>& dynamicDefines)
  {
    std::string vertexShaderSource = loadShaderSource(vertexShaderFilename, dynamicDefines);
    std::string fragmentShaderSource = loadShaderSource(fragmentShaderFilename, dynamicDefines);
    std::string geometryShaderSource = loadShaderSource(geometryShaderFilename, dynamicDefines);
    std::string tesselationCTRLShaderSource = loadShaderSource(tesselationCTRLShaderFilename, dynamicDefines);
    std::string tesselationEVALShaderSource = loadShaderSource(tesselationEVALShaderFilename, dynamicDefines);
    std::string computeShaderSource = loadShaderSource(computeShaderFilename, dynamicDefines);

    ResourceHandle shaderHandle;

    bool noRenderShader = (
                            computeShaderFilename == std::string() && 
                            (
                              (vertexShaderSource == std::string() || fragmentShaderSource == std::string()) || 
                              (geometryShaderFilename != std::string() && geometryShaderSource == std::string()) ||
                              (tesselationCTRLShaderFilename != std::string() && tesselationCTRLShaderSource == std::string()) ||
                              (tesselationEVALShaderFilename != std::string() && computeShaderSource == std::string())
                            )
                          );

    bool noComputeShader = (computeShaderFilename != std::string() && computeShaderSource == std::string());

    if(noRenderShader || noComputeShader)
    {
      std::cout << "ERROR, couldn't open file: " << shaderName << std::endl;
      if(noRenderShader)
      {
        shaderHandle = m_shaderManager->getDefaultResource();
      }
      else
      {
        assert("IMPLEMENT DEFAULT COMPUTE SHADER!");
        shaderHandle;
      }
    }
    else
    {
      shaderHandle = m_shaderManager->addObject(Shader(shaderName, vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource, computeShaderSource));
    }

    return shaderHandle;
  }

  std::string ShaderLoader::loadShaderSource(std::string shaderFilename, std::vector<std::string>& dynamicDefines)
  {
    if(shaderFilename == std::string())
    {
      return std::string();
    }

    std::string shaderPath = m_shaderManager->getPath();

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