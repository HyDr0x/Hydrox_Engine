#include "Hydrox/Loader/MaterialLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Loader/RenderShaderLoader.h"
#include "Hydrox/Loader/ILDevilLoader.h"

namespace he
{
  MaterialLoader::MaterialLoader() : m_materialManager(MaterialManager::getInstance()),
                                     m_textureManager(TextureManager::getInstance()),
                                     m_renderShaderManager(RenderShaderManager::getInstance())
  {
  }

  MaterialLoader::~MaterialLoader()
  {
  }

  ResourceHandle MaterialLoader::loadMaterial(std::string materialFilename)
  {
    if(materialFilename == std::string())
    {
      return ResourceHandle(~0, nullptr);
    }

    ResourceHandle materialHandle;

    std::string vertexFileName, fragmentFilename, geometryFilename, tesselationControlFilename, tesselationEvaluationFilename;
    std::string diffuseFilename[m_TEXNUMBER], normalFilename[m_TEXNUMBER], specularFilename[m_TEXNUMBER], displacementFilename[m_TEXNUMBER];

    std::string materialPath = m_materialManager->getPath();
    materialFilename = materialPath + materialFilename;
    std::ifstream file(materialFilename);
	  std::string line;

	  if(file.is_open())
    {
      Material::MaterialData materialData;
		  while(!file.eof())
		  {
			  std::getline(file, line);

        if(line.empty() || line[0] == '#')//skip comments
        {
          continue;
        }

        if(line.find("Diffuse Strength") != std::string::npos)
        {
          std::getline(file, line);
          std::stringstream stream(line);
          stream >> materialData.diffuseStrength;
        }

        if(line.find("Specular Strength") != std::string::npos)
        {
          std::getline(file, line);
          std::stringstream stream(line);
          stream >> materialData.specularStrength;
        }

        if(line.find("Ambient Strength") != std::string::npos)
        {
          std::getline(file, line);
          std::stringstream stream(line);
          stream >> materialData.ambientStrength;
        }

        if(line.find("Specular Exponent") != std::string::npos)
        {
          std::getline(file, line);
          std::stringstream stream(line);
          stream >> materialData.specularExponent;
        }

        if(line.find("Transparency") != std::string::npos)
        {
          std::getline(file, line);
          std::stringstream stream(line);
          stream >> materialData.transparency;
        }

        if(line.find("Mesh Color") != std::string::npos)
        {
          std::getline(file, line);
          std::stringstream stream(line);
          stream >> materialData.color[0];

          std::getline(file, line);
          std::stringstream stream2(line);
          stream2 >> materialData.color[1];

          std::getline(file, line);
          std::stringstream stream3(line);
          stream3 >> materialData.color[2];
        }

        /////////////////////////TEXTURES/////////////////////////
        if(line.find("Diffuse Color") != std::string::npos)
        {
          for(unsigned int i = 0; i < m_TEXNUMBER; i++)
          {
            std::getline(file, line);
            if(line != std::string("NULL"))
            {
              diffuseFilename[i] = line;
            }
          }
        }

        if(line.find("Normal Map") != std::string::npos)
        {
          for(unsigned int i = 0; i < m_TEXNUMBER; i++)
          {
            std::getline(file, line);
            if(line != std::string("NULL"))
            {
              normalFilename[i] = line;
            }
          }
        }

        if(line.find("Specular Map") != std::string::npos)
        {
          for(unsigned int i = 0; i < m_TEXNUMBER; i++)
          {
            std::getline(file, line);
            if(line != std::string("NULL"))
            {
              specularFilename[i] = line;
            }
          }
        }

        if(line.find("Displacement Map") != std::string::npos)
        {
          for(unsigned int i = 0; i < m_TEXNUMBER; i++)
          {
            std::getline(file, line);
            if(line != std::string("NULL"))
            {
              displacementFilename[i] = line;
            }
          }
        }

        /////////////////////////SHADER/////////////////////////
        if(line.find("Vertex Shader") != std::string::npos)
        {
          std::getline(file, line);
          if(line != std::string("NULL"))
          {
            vertexFileName = line;
          }
        }

        if(line.find("Fragment Shader") != std::string::npos)
        {
          std::getline(file, line);
          if(line != std::string("NULL"))
          {
            fragmentFilename = line;
          }
        }

        if(line.find("Geometry Shader") != std::string::npos)
        {
          std::getline(file, line);
          if(line != std::string("NULL"))
          {
            geometryFilename = line;
          }
        }

        if(line.find("Tesselation Control Shader") != std::string::npos)
        {
          std::getline(file, line);
          if(line != std::string("NULL"))
          {
            tesselationControlFilename = line;
          }
        }

        if(line.find("Tesselation Evaluation Shader") != std::string::npos)
        {
          std::getline(file, line);
          if(line != std::string("NULL"))
          {
            tesselationEvaluationFilename = line;
          }
        }
		  }

      RenderShaderLoader renderShaderLoader;
      ResourceHandle shaderHandle = renderShaderLoader.loadShader(materialFilename + std::string("_Shader"), vertexFileName, fragmentFilename, geometryFilename, tesselationControlFilename, tesselationEvaluationFilename);

      ILDevilLoader textureLoader;

      std::vector<std::vector<ResourceHandle>> textureHandles(4);
      for(unsigned int j = 0; j < m_TEXNUMBER; j++)
      {
        if(diffuseFilename[j] != std::string())
        {
          textureHandles[0].push_back(textureLoader.load(diffuseFilename[j], GL_TEXTURE_2D));
        }
      }

      for(unsigned int j = 0; j < m_TEXNUMBER; j++)
      {
        if(normalFilename[j] != std::string())
        {
          textureHandles[1].push_back(textureLoader.load(normalFilename[j], GL_TEXTURE_2D));
        }
      }

      for(unsigned int j = 0; j < m_TEXNUMBER; j++)
      {
        if(specularFilename[j] != std::string())
        {
          textureHandles[2].push_back(textureLoader.load(specularFilename[j], GL_TEXTURE_2D));
        }
      }

      for(unsigned int j = 0; j < m_TEXNUMBER; j++)
      {
        if(displacementFilename[j] != std::string())
        {
          textureHandles[3].push_back(textureLoader.load(displacementFilename[j], GL_TEXTURE_2D));
        }
      }

      materialHandle = m_materialManager->addObject(Material(materialData, textureHandles, shaderHandle));
    }
	  else//wrong filename or file does not exist
	  {
		  file.close();

		  std::cout << "Error: couldn't open material source file " << materialFilename << "." << std::endl;


      std::string vertexSource = "#version 430 core\n\
                                layout(location = 4) uniform mat4 MVP;\n\
                                \n\
                                layout(location = 0) in vec3 in_Pos;\n\
                                \n\
                                void main()\n\
                                {\n\
	                                gl_Position = MVP * vec4(in_Pos, 1);\n\
                                }";

      std::string fragmentSource = "#version 430 core\n\
                                    layout(early_fragment_tests) in;\n\
                                    \n\
                                    out vec4 color;\n\
                                    \n\
                                    void main()\n\
                                    {\n\
	                                    color = vec4(1,1,1,1);\n\
                                    }";

      ResourceHandle renderShaderHandle = m_renderShaderManager->addObject(RenderShader(std::string("defaultRenderShader"), vertexSource, fragmentSource));

      return m_materialManager->addObject(Material(Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f, false), std::vector<std::vector<ResourceHandle>>(), renderShaderHandle));
	  }

	  file.close();

    return materialHandle;
  }

  ResourceHandle MaterialLoader::getDefaultMaterial()
  {
    return MaterialManager::getInstance()->addObject(Material(Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f, false), std::vector<std::vector<ResourceHandle>>(), RenderShaderLoader::getDefaultRenderShader()));
  }
}