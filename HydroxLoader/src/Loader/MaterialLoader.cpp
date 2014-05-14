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
                                                                               m_renderShaderManager(singletonManager->getService<renderer::RenderShaderManager>())
    {
    }

    MaterialLoader::~MaterialLoader()
    {
    }

    util::ResourceHandle MaterialLoader::loadResource(std::string filename)
    {
      if(filename == std::string())
      {
        return util::ResourceHandle();
      }

      util::ResourceHandle materialHandle;

      std::string shaderFilename;
      std::vector<bool> shaderPrograms(4, false);
      std::vector<std::vector<std::string>> textureFilenames(renderer::Material::TEXTURETYPENUM);
      std::vector<std::string> textureTypes(renderer::Material::TEXTURETYPENUM);
      textureTypes[0] = "Diffuse Color";
      textureTypes[1] = "Normal Map";
      textureTypes[2] = "Specular Map";
      textureTypes[3] = "Displacement Map";

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
          for(unsigned int i = 0; i < textureFilenames.size(); i++)
          {
            if(line.find(textureTypes[i]) != std::string::npos)
            {
              for(unsigned int j = 0; j < renderer::Material::TEXTURENUMBER; j++)
              {
                std::getline(file, line);
                if(line != std::string("NULL"))
                {
                  textureFilenames[i].push_back(line);
                }
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
        util::ResourceHandle shaderHandle = renderShaderLoader.loadResource(shaderFilename);

        ILDevilLoader textureLoader(m_singletonManager);

        std::vector<std::vector<util::ResourceHandle>> textureHandles(textureFilenames.size());

        for(unsigned int i = 0; i < textureFilenames.size(); i++)
        {
          for(unsigned int j = 0; j < textureFilenames[i].size(); j++)
          {
            if(textureFilenames[i][j] != std::string())
            {
              textureHandles[i].push_back(textureLoader.loadResource(textureFilenames[i][j]));
            }
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

    util::ResourceHandle MaterialLoader::getDefaultResource() const
    {
      RenderShaderLoader renderShaderLoader(m_singletonManager);
      return m_materialManager->addObject(renderer::Material(renderer::Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f), std::vector<std::vector<util::ResourceHandle>>(renderer::Material::TEXTURETYPENUM), renderShaderLoader.getDefaultResource()));
    }
  }
}