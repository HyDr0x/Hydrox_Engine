#include "Saver/ILDevilSaver.h"

#include <IL/il.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Renderer/Resources/ResourceManager.hpp>
#include <Renderer/Resources/Texture2D.h>

namespace he
{
	namespace saver
	{
    void ILDevilSaver::save(std::string path, std::string filename, util::ResourceHandle textureHandle, util::SingletonManager *singletonManager)
    {
      renderer::Texture2D *texture = singletonManager->getService<renderer::TextureManager>()->getObject(textureHandle);

      ILuint tex;
	    ilGenImages(1, &tex);
	    ilBindImage(tex);

      std::vector<GLubyte> imageData(texture->getTextureSize());
      texture->getTextureData(&imageData[0]);
      ilTexImage(texture->getResolution()[0], texture->getResolution()[1], 0, texture->getChannelNumber(), texture->getFormat(), texture->getType(), &imageData[0]);

      ilSaveImage((path + filename).c_str());

      ilBindImage(0);
      ilDeleteImages(1, &tex);
    }
  }
}