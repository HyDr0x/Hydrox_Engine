#include "Saver/MaterialSaver.h"

#include <sstream>
#include <fstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Material.h>
#include <DataBase/Texture2D.h>

#include <Shader/ShaderContainer.h>

#include "Saver/RenderShaderSaver.h"
#include "Saver/ILDevilSaver.h"

namespace he
{
  namespace saver
  {
    void MaterialSaver::save(std::string path, std::string filename, const util::ResourceHandle materialHandle, util::SingletonManager *singletonManager)
    {
      std::map<MaterialFileKeywords, std::string> materialFileKeywords;
      materialFileKeywords[METALNESS] = "Metalness";
      materialFileKeywords[REFLECTANCE] = "Reflectance";
      materialFileKeywords[ROUGHNESS0] = "Roughness0";
      materialFileKeywords[ROUGHNESS1] = "Roughness1";

      materialFileKeywords[TRANSPARENCY] = "Transparency";
      materialFileKeywords[RENDERDEBUG] = "Debug";
      materialFileKeywords[UNICOLOR] = "Unicolor";

      materialFileKeywords[DIFFUSETEXTURE] = "Diffuse Map";
      materialFileKeywords[NORMALMAP] = "Normal Map";
      materialFileKeywords[METALNESSMAP] = "Metalness Map";
      materialFileKeywords[ROUGHNESSMAP] = "Roughness Map";

      util::SharedPointer<db::TextureManager> textureManager = singletonManager->getService<db::TextureManager>();
      util::SharedPointer<db::MaterialManager> materialManager = singletonManager->getService<db::MaterialManager>();

      db::Material *material = materialManager->getObject(materialHandle);

      std::ofstream fileStream;
      fileStream.open(path + filename + std::string(".material"), std::ofstream::out | std::ofstream::trunc);

      /////////////////////////MATERIAL DATA/////////////////////////
      fileStream << materialFileKeywords[METALNESS] << std::endl;
      fileStream << material->getMaterialData().metallic << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[REFLECTANCE] << std::endl;
      fileStream << material->getMaterialData().reflectance << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[ROUGHNESS0] << std::endl;
      fileStream << material->getMaterialData().roughness0 << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[ROUGHNESS1] << std::endl;
      fileStream << material->getMaterialData().roughness1 << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[TRANSPARENCY] << std::endl;
      fileStream << material->getTransparency() << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[RENDERDEBUG] << std::endl;
      fileStream << material->getDebug() << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[UNICOLOR] << std::endl;
      fileStream << material->getMaterialData().color << std::endl;
      fileStream << std::endl;

      /////////////////////////TEXTURES/////////////////////////
      for(unsigned int i = 0; i < db::Material::TEXTURETYPENUM; i++)
      {
        fileStream << materialFileKeywords[MaterialFileKeywords(i + TEXTUREOFFSET + 1)] << std::endl;
        for(unsigned int j = 0; j < db::Material::TEXTURETYPENUM; j++)
        {
          if(j < material->getTextureNumber((db::Material::TextureType)i))
          {
            db::Texture2D *texture = textureManager->getObject(material->getTextureHandle((db::Material::TextureType)i, j));
            std::stringstream textureFilename; 
            textureFilename << filename << materialFileKeywords[MaterialFileKeywords(i + TEXTUREOFFSET + 1)] << j;

            ILDevilSaver::save(path, textureFilename.str(), material->getTextureHandle((db::Material::TextureType)i, j), singletonManager);

            fileStream << path << textureFilename.str() << std::string(".png") << std::endl;
          }
          else
          {
            fileStream << "NULL" << std::endl;
          }
        }
        fileStream << std::endl;
      }

      fileStream.close();
    }
  }
}
