#include "Loader/MaterialLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Renderer/Resources/Material.h>
#include "Loader/RenderShaderLoader.h"
#include "Loader/ILDevilLoader.h"

namespace he
{
  namespace loader
  {
    MaterialLoader::MaterialLoader(util::SingletonManager *singletonManager) : ResourceLoader(singletonManager),
                                                                               m_materialManager(singletonManager->getService<renderer::MaterialManager>()),
                                                                               m_textureManager(singletonManager->getService<renderer::TextureManager>()),
                                                                               m_renderShaderManager(singletonManager->getService<renderer::RenderShaderManager>()),
                                                                               m_texNumber(4)
    {
    }

    MaterialLoader::~MaterialLoader()
    {
    }

    util::ResourceHandle MaterialLoader::loadResource(std::string filename)
    {
      if(filename == std::string())
      {
        return util::ResourceHandle(~0, nullptr);
      }

      util::ResourceHandle materialHandle;

      std::string shaderFilename;
      std::vector<bool> shaderPrograms(4, false);
      std::vector<std::string> diffuseFilename(m_texNumber), normalFilename(m_texNumber), specularFilename(m_texNumber), displacementFilename(m_texNumber);

      std::ifstream file(filename);
	    std::string line;

	    if(file.is_open())
      {
        renderer::Material::MaterialData materialData;
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

          /////////////////////////TEXTURES/////////////////////////
          if(line.find("Diffuse Color") != std::string::npos)
          {
            for(unsigned int i = 0; i < m_texNumber; i++)
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
            for(unsigned int i = 0; i < m_texNumber; i++)
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
            for(unsigned int i = 0; i < m_texNumber; i++)
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
            for(unsigned int i = 0; i < m_texNumber; i++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                displacementFilename[i] = line;
              }
            }
          }

          /////////////////////////SHADER/////////////////////////
          if(line.find("Shader Name") != std::string::npos)
          {
            std::getline(file, line);
            shaderFilename = line;
          }

          if(line.find("Fragment Shader") != std::string::npos)
          {
            std::getline(file, line);
            if(line == std::string("TRUE"))
            {
              shaderPrograms[0] = true;
            }
          }

          if(line.find("Geometry Shader") != std::string::npos)
          {
            std::getline(file, line);
            if(line == std::string("TRUE"))
            {
              shaderPrograms[1] = true;
            }
          }

          if(line.find("Tesselation Control Shader") != std::string::npos)
          {
            std::getline(file, line);
            if(line == std::string("TRUE"))
            {
              shaderPrograms[2] = true;
            }
          }

          if(line.find("Tesselation Evaluation Shader") != std::string::npos)
          {
            std::getline(file, line);
            if(line == std::string("TRUE"))
            {
              shaderPrograms[3] = true;
            }
          }
		    }

        RenderShaderLoader renderShaderLoader(m_singletonManager);
        renderShaderLoader.setUsedShaderPrograms(shaderPrograms[0], shaderPrograms[1], shaderPrograms[2], shaderPrograms[3]);
        util::ResourceHandle shaderHandle = renderShaderLoader.loadResource(shaderFilename);

        ILDevilLoader textureLoader(m_singletonManager);

        std::vector<std::vector<util::ResourceHandle>> textureHandles(4);
        for(unsigned int j = 0; j < m_texNumber; j++)
        {
          if(diffuseFilename[j] != std::string())
          {
            textureHandles[0].push_back(textureLoader.loadResource(diffuseFilename[j]));
          }
        }

        for(unsigned int j = 0; j < m_texNumber; j++)
        {
          if(normalFilename[j] != std::string())
          {
            textureHandles[1].push_back(textureLoader.loadResource(normalFilename[j]));
          }
        }

        for(unsigned int j = 0; j < m_texNumber; j++)
        {
          if(specularFilename[j] != std::string())
          {
            textureHandles[2].push_back(textureLoader.loadResource(specularFilename[j]));
          }
        }

        for(unsigned int j = 0; j < m_texNumber; j++)
        {
          if(displacementFilename[j] != std::string())
          {
            textureHandles[3].push_back(textureLoader.loadResource(displacementFilename[j]));
          }
        }

        materialHandle = m_materialManager->addObject(renderer::Material(materialData, textureHandles, shaderHandle));
      }
	    else//wrong filename or file does not exist
	    {
		    file.close();

		    std::cout << "Error: couldn't open material source file " << filename[0] << "." << std::endl;

        return getDefaultResource();
	    }

	    file.close();

      return materialHandle;
    }

    util::ResourceHandle MaterialLoader::getDefaultResource()
    {
      RenderShaderLoader renderShaderLoader(m_singletonManager);
      return m_materialManager->addObject(renderer::Material(renderer::Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f), std::vector<std::vector<util::ResourceHandle>>(4), renderShaderLoader.getDefaultResource()));
    }
  }
}