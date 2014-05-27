#include "Saver/MaterialSaver.h"

#include <sstream>
#include <fstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Renderer/Resources/Material.h>
#include <Renderer/Resources/Texture2D.h>
#include <Renderer/Resources/RenderShader.h>

#include "Saver/NodeWrapper.h"
#include "Saver/NodeWrapperMapper.h"

#include "Saver/RenderShaderSaver.h"
#include "Saver/ILDevilSaver.h"

namespace he
{
	namespace saver
	{    void MaterialSaver::save(std::string path, std::string filename, const util::ResourceHandle materialHandle, util::SingletonManager *singletonManager)
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

      materialFileKeywords[SHADERNAME] = "Shader Name";

      renderer::TextureManager *textureManager = singletonManager->getService<renderer::TextureManager>();      renderer::RenderShaderManager *renderShaderManager = singletonManager->getService<renderer::RenderShaderManager>();
      renderer::MaterialManager *materialManager = singletonManager->getService<renderer::MaterialManager>();

      renderer::Material *material = materialManager->getObject(materialHandle);

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
      for(unsigned int i = 0; i < renderer::Material::TEXTURETYPENUM; i++)
      {
        fileStream << materialFileKeywords[MaterialFileKeywords(i + TEXTUREOFFSET + 1)] << std::endl;
        for(unsigned int j = 0; j < renderer::Material::TEXTURENUMBER; j++)
        {
          if(j < material->getTextureNumber((renderer::Material::TextureType)i))
          {
            renderer::Texture2D *texture = textureManager->getObject(material->getTextureHandle((renderer::Material::TextureType)i, j));
            std::stringstream textureFilename; 
            textureFilename << filename << materialFileKeywords[MaterialFileKeywords(i + TEXTUREOFFSET + 1)] << j;

            ILDevilSaver::save(path, textureFilename.str(), material->getTextureHandle((renderer::Material::TextureType)i, j), singletonManager);

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
      fileStream << materialFileKeywords[SHADERNAME] << std::endl;
      std::stringstream shaderFilename;

      shaderFilename << filename << "_shader";
      fileStream << path << shaderFilename.str() << std::endl;

      fileStream << std::endl;

      RenderShaderSaver::save(path, shaderFilename.str(), material->getShaderHandle(), singletonManager);
      
      fileStream.close();
    }
	}
}
