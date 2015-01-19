#include "Saver/MaterialSaver.h"

#include <sstream>
#include <fstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Material.h>
#include <DataBase/Texture2D.h>
#include <DataBase/RenderShader.h>

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

      materialFileKeywords[DIFFUSETEXTURE] = "Diffuse Map";
      materialFileKeywords[NORMALMAP] = "Normal Map";
      materialFileKeywords[SPECULARMAP] = "Specular Map";
      materialFileKeywords[DISPLACEMENTMAP] = "Displacement Map";

      materialFileKeywords[SHADOWSHADERNAME] = "ShadowShader Name";
      materialFileKeywords[SHADERNAME] = "Shader Name";

      util::SharedPointer<db::TextureManager> textureManager = singletonManager->getService<db::TextureManager>();
      util::SharedPointer<db::RenderShaderManager> renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
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

      /////////////////////////TEXTURES/////////////////////////
      for(unsigned int i = 0; i < db::Material::TEXTURETYPENUM; i++)
      {
        fileStream << materialFileKeywords[MaterialFileKeywords(i + TEXTUREOFFSET + 1)] << std::endl;
        for(unsigned int j = 0; j < db::Material::TEXTURENUMBER; j++)
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

      /////////////////////////SHADER/////////////////////////
      he::db::RenderShader *shadowShader = renderShaderManager->getObject(material->getShadowShaderHandle());
      he::db::RenderShader *renderShader = renderShaderManager->getObject(material->getShadowShaderHandle());

      fileStream << materialFileKeywords[SHADOWSHADERNAME] << std::endl;
      std::stringstream shadowShaderFilename;
      shadowShaderFilename << filename << "_shadowshader";
      for(unsigned int i = 0; i < shadowShader->getShaderSources().size(); i++)
      {
        if(!shadowShader->getShaderSources()[i].empty())
        {
          fileStream << path << shadowShaderFilename.str() << std::endl;
        }
        else
        {
          fileStream << "" << std::endl;
        }
      }

      fileStream << materialFileKeywords[SHADERNAME] << std::endl;
      std::stringstream shaderFilename;
      shaderFilename << filename << "_shader";
      for(unsigned int i = 0; i < renderShader->getShaderSources().size(); i++)
      {
        if(!renderShader->getShaderSources()[i].empty())
        {
          fileStream << path << shaderFilename.str() << std::endl;
        }
        else
        {
          fileStream << "" << std::endl;
        }
      }

      fileStream << std::endl;

      RenderShaderSaver::save(path, shadowShaderFilename.str(), material->getShadowShaderHandle(), singletonManager);
      RenderShaderSaver::save(path, shaderFilename.str(), material->getShaderHandle(), singletonManager);
      
      fileStream.close();
    }
  }
}
