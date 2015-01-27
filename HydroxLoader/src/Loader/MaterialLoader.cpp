#include "Loader/MaterialLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Material.h>

#include "Loader/RenderShaderLoader.h"
#include "Loader/ILDevilLoader.h"

namespace he
{
  namespace loader
  {
    MaterialLoader::MaterialLoader(util::SingletonManager *singletonManager) : ResourceLoader(singletonManager),
                                                                               m_materialManager(singletonManager->getService<db::MaterialManager>()),
                                                                               m_textureManager(singletonManager->getService<db::TextureManager>())
    {
      m_materialFileKeywords["Diffuse Strength"] = DIFFUSESTRENGTH;
      m_materialFileKeywords["Specular Strength"] = SPECULARSTRENGTH;
      m_materialFileKeywords["Ambient Strength"] = AMBIENTSTRENGTH;
      m_materialFileKeywords["Specular Exponent"] = SPECULAREXPONENT;

      m_materialFileKeywords["Diffuse Map"] = DIFFUSETEXTURE;
      m_materialFileKeywords["Normal Map"] = NORMALMAP;
      m_materialFileKeywords["Specular Map"] = SPECULARMAP;
      m_materialFileKeywords["Displacement Map"] = DISPLACEMENTMAP;
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

      std::vector<std::vector<std::string>> textureFilenames(db::Material::TEXTURETYPENUM);

      std::ifstream file(filename);
      std::string line;

      if(file.is_open())
      {
        db::Material::MaterialData materialData;
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
            materialData.diffuseStrength = std::strtod(line.c_str(), nullptr);
            break;
          case SPECULARSTRENGTH:
            std::getline(file, line);
            materialData.specularStrength = std::strtod(line.c_str(), nullptr);
            break;
          case AMBIENTSTRENGTH:
            std::getline(file, line);
            materialData.ambientStrength = std::strtod(line.c_str(), nullptr);
            break;
          case SPECULAREXPONENT:
            std::getline(file, line);
            materialData.specularExponent = std::strtod(line.c_str(), nullptr);
            break;
          case DIFFUSETEXTURE:
            for(unsigned int j = 0; j < db::Material::TEXTURENUMBER; j++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                textureFilenames[0].push_back(line);
              }
            }
            break;
          case NORMALMAP:
            for(unsigned int j = 0; j < db::Material::TEXTURENUMBER; j++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                textureFilenames[1].push_back(line);
              }
            }
            break;
          case SPECULARMAP:
            for(unsigned int j = 0; j < db::Material::TEXTURENUMBER; j++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                textureFilenames[2].push_back(line);
              }
            }
            break;
          case DISPLACEMENTMAP:
            for(unsigned int j = 0; j < db::Material::TEXTURENUMBER; j++)
            {
              std::getline(file, line);
              if(line != std::string("NULL"))
              {
                textureFilenames[3].push_back(line);
              }
            }
            break;
          case DEFAULT:
          default:
            continue;//unknown command
          }
        }

        std::vector<uint64_t> hashes;

        ILDevilLoader textureLoader(m_singletonManager);

        std::vector<std::vector<util::ResourceHandle>> textureHandles(textureFilenames.size());

        for(unsigned int i = 0; i < textureFilenames.size(); i++)
        {
          for(unsigned int j = 0; j < textureFilenames[i].size(); j++)
          {
            if(textureFilenames[i][j] != std::string())
            {
              textureHandles[i].push_back(textureLoader.loadResource(textureFilenames[i][j]));
              db::Texture2D *texture = m_singletonManager->getService<db::TextureManager>()->getObject(textureHandles[i][j]);
              hashes.push_back(texture->getHash());
            }
          }
        }

        materialHandle = m_materialManager->addObject(db::Material(materialData, textureHandles, hashes, false));
      }
      else//wrong filename or file does not exist
      {
        file.close();

        std::cerr << "Error: couldn't open material source file " << filename[0] << "." << std::endl;

        return getDefaultResource();
      }

      file.close();

      return materialHandle;
    }

    util::ResourceHandle MaterialLoader::getDefaultResource() const
    {
      std::vector<uint64_t> hashes;
      return m_materialManager->addObject(db::Material(db::Material::MaterialData(1.0f, 1.0f, 1.0f, 1.0f), std::vector<std::vector<util::ResourceHandle>>(db::Material::TEXTURETYPENUM), hashes, false));
    }
  }
}