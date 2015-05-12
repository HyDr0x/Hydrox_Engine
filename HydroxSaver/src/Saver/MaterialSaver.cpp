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
      materialFileKeywords[DIFFUSESTRENGTH] = "Diffuse Strength";
      materialFileKeywords[SPECULARSTRENGTH] = "Specular Strength";
      materialFileKeywords[AMBIENTSTRENGTH] = "Ambient Strength";
      materialFileKeywords[SPECULAREXPONENT] = "Specular Exponent";

      materialFileKeywords[TRANSPARENCY] = "Transparency";
      materialFileKeywords[RENDERDEBUG] = "Debug";
      materialFileKeywords[UNICOLOR] = "Unicolor";

      materialFileKeywords[DIFFUSETEXTURE] = "Diffuse Map";
      materialFileKeywords[NORMALMAP] = "Normal Map";
      materialFileKeywords[SPECULARMAP] = "Specular Map";
      materialFileKeywords[DISPLACEMENTMAP] = "Displacement Map";

      util::SharedPointer<db::TextureManager> textureManager = singletonManager->getService<db::TextureManager>();
      util::SharedPointer<db::MaterialManager> materialManager = singletonManager->getService<db::MaterialManager>();

      db::Material *material = materialManager->getObject(materialHandle);

      std::ofstream fileStream;
      fileStream.open(path + filename + std::string(".material"), std::ofstream::out | std::ofstream::trunc);

      /////////////////////////MATERIAL DATA/////////////////////////
      fileStream << materialFileKeywords[DIFFUSESTRENGTH] << std::endl;
      fileStream << material->getMaterialData().diffuseStrength << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[SPECULARSTRENGTH] << std::endl;
      fileStream << material->getMaterialData().specularStrength << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[AMBIENTSTRENGTH] << std::endl;
      fileStream << material->getMaterialData().ambientStrength << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[SPECULAREXPONENT] << std::endl;
      fileStream << material->getMaterialData().specularExponent << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[TRANSPARENCY] << std::endl;
      fileStream << material->getTransparency() << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[RENDERDEBUG] << std::endl;
      fileStream << material->getDebug() << std::endl;
      fileStream << std::endl;
      fileStream << materialFileKeywords[UNICOLOR] << std::endl;
      fileStream << material->getUniColor() << std::endl;
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
