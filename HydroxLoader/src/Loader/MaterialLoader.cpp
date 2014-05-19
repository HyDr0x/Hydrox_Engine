#include "Loader/MaterialLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

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
      m_materialFileKeywords["Diffuse Strength"] = DIFFUSESTRENGTH;
      m_materialFileKeywords["Specular Strength"] = SPECULARSTRENGTH;
      m_materialFileKeywords["Ambient Strength"] = AMBIENTSTRENGTH;
      m_materialFileKeywords["Specular Exponent"] = SPECULAREXPONENT;

      m_materialFileKeywords["Diffuse Map"] = DIFFUSETEXTURE;
      m_materialFileKeywords["Normal Map"] = NORMALMAP;
      m_materialFileKeywords["Specular Map"] = SPECULARMAP;
      m_materialFileKeywords["Displacement Map"] = DISPLACEMENTMAP;

      m_materialFileKeywords["Shader Name"] = SHADERNAME;

      m_materialFileKeywords["Fragment Shader"] = FRAGMENTSHADER;
      m_materialFileKeywords["Geometry Shader"] = GEOMETRYSHADER;
      m_materialFileKeywords["Tesselation Control Shader"] = TESSCONTROLSHADER;
      m_materialFileKeywords["Tesselation Evaluation Shader"] = TESSEVALSHADER;
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

          switch(m_materialFileKeywords[line])
          {
          case DIFFUSESTRENGTH:
            std::getline(file, line);
            materialData.diffuseStrength = std::strtol(line.c_str(), nullptr, 0);
            break;
          case SPECULARSTRENGTH:
            std::getline(file, line);
            materialData.specularStrength = std::strtol(line.c_str(), nullptr, 0);
            break;
          case AMBIENTSTRENGTH:
            std::getline(file, line);
            materialData.ambientStrength = std::strtol(line.c_str(), nullptr, 0);
            break;
          case SPECULAREXPONENT:
            std::getline(file, line);
            materialData.specularExponent = std::strtol(line.c_str(), nullptr, 0);
            break;
          case DIFFUSETEXTURE:
            for(unsigned int j = 0; j < renderer::Material::TEXTURENUMBER; j++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                textureFilenames[0].push_back(line);
              }
            }
            break;
          case NORMALMAP:
            for(unsigned int j = 0; j < renderer::Material::TEXTURENUMBER; j++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                textureFilenames[1].push_back(line);
              }
            }
            break;
          case SPECULARMAP:
            for(unsigned int j = 0; j < renderer::Material::TEXTURENUMBER; j++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                textureFilenames[2].push_back(line);
              }
            }
            break;
          case DISPLACEMENTMAP:
            for(unsigned int j = 0; j < renderer::Material::TEXTURENUMBER; j++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                textureFilenames[3].push_back(line);
              }
            }
            break;
          case SHADERNAME:
            std::getline(file, line);
            shaderFilename = line;
            break;
          case FRAGMENTSHADER:
            std::getline(file, line);
            if(line == std::string("TRUE"))
            {
              shaderPrograms[0] = true;
            }
            break;
          case GEOMETRYSHADER:
            std::getline(file, line);
            if(line == std::string("TRUE"))
            {
              shaderPrograms[1] = true;
            }
            break;
          case TESSCONTROLSHADER:
            std::getline(file, line);
            if(line == std::string("TRUE"))
            {
              shaderPrograms[2] = true;
            }
            break;
          case TESSEVALSHADER:
            std::getline(file, line);
            if(line == std::string("TRUE"))
            {
              shaderPrograms[3] = true;
            }
            break;
          case DEFAULT:
          default:
            continue;//unknown command
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

        materialHandle = m_materialManager->addObject(renderer::Material(materialData, textureHandles, shaderHandle, false));
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
      return m_materialManager->addObject(renderer::Material(renderer::Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f), std::vector<std::vector<util::ResourceHandle>>(renderer::Material::TEXTURETYPENUM), renderShaderLoader.getDefaultResource(), false));
    }
  }
}