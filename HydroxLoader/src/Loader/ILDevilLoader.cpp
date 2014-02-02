#include "Loader/ILDevilLoader.h"

#include <Renderer/Resources/Texture.h>

namespace he
{
  namespace loader
  {
    ILDevilLoader::ILDevilLoader(renderer::TextureManager *textureManager) : m_textureManager(textureManager)
    {
    }

    ILDevilLoader::ILDevilLoader(const ILDevilLoader& o )
    {
      m_textureManager = o.m_textureManager;
    }

    ILDevilLoader& ILDevilLoader::operator=(const ILDevilLoader& o)
    {
      m_textureManager = o.m_textureManager;

      return *this;
    }

    ILDevilLoader::~ILDevilLoader()
    {
    }

    util::ResourceHandle ILDevilLoader::load(std::string path, std::string filename, GLenum target)
    {
      util::ResourceHandle tmpTextureID;
      GLsizei width, height;
      GLenum internalFormat;
      GLenum format;
      GLenum type;

      ILuint tex;
	    ilGenImages(1, &tex);
	    ilBindImage(tex);
      {
        std::string texturePath = path;
        texturePath += filename;
		    ILboolean success = ilLoadImage(texturePath.c_str());

		    if(!success)
        {
          std::cout << "ERROR, couldn't open file: " << texturePath << std::endl;

          return getDefaultTexture();
        }
        else
        {
          GLuint componentNumber = 0;
          getImageInformations(width, height, internalFormat, format, type, componentNumber);

          //success = ilConvertImage(format, type);
		      //if(!success)
          //{
		      //	printf("ERROR, couldn't convert file %s\n", filename);
          //}

          int bytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
          bytesPerPixel = ilGetInteger(IL_FORMAT_MODE);
        
          tmpTextureID = m_textureManager->addObject(renderer::Texture(width, height, target, type, internalFormat, format, ilGetData()));
        }
      }
      ilBindImage(0);
      ilDeleteImages(1, &tex);

      return tmpTextureID;
    }

    util::ResourceHandle ILDevilLoader::getDefaultTexture()
    {
      util::Vector<float, 3> textureData = util::Vector<float, 3>(0.0f, 1.0f, 0.0f);
      return m_textureManager->addObject(renderer::Texture(1, 1, GL_TEXTURE_2D, GL_FLOAT, GL_RGB8, GL_RGB, &textureData[0]));
    }

    void ILDevilLoader::getImageInformations(GLsizei& width, GLsizei& height, GLenum& internalFormat, GLenum& format, GLenum& type, GLuint& components)
    {
      width = ilGetInteger(IL_IMAGE_WIDTH);
      height = ilGetInteger(IL_IMAGE_HEIGHT);

      switch(ilGetInteger(IL_IMAGE_FORMAT))
      {
      case IL_RGB:
        format = GL_RGB;
        break;
      case IL_RGBA:
        format = GL_RGBA;
        break;
      case IL_BGR:
        format = GL_BGR;
        break;
      case IL_BGRA:
        format = GL_BGRA;
        break;
      default:
        format = GL_RGBA;
      }

      switch(format)
      {
      case GL_RGB:
      case GL_BGR:
        components = 3;
        switch(ilGetInteger(IL_IMAGE_BITS_PER_PIXEL))
        {
        case 48:
          internalFormat = GL_RGB16;
          break;
        case 96:
          internalFormat = GL_RGB32F;
          break;
        case 24:
        default:
          internalFormat = GL_RGB8;
        }
        break;
      case GL_RGBA:
      case GL_BGRA:
        components = 4;
        switch(ilGetInteger(IL_IMAGE_BITS_PER_PIXEL))
        {
        case 64:
          internalFormat = GL_RGBA16;
          break;
        case 128:
          internalFormat = GL_RGBA32F;
          break;
        case 32:
        default:
          internalFormat = GL_RGBA8;
        }
        break;
      }

      switch(ilGetInteger(IL_IMAGE_TYPE))
      {
      case IL_UNSIGNED_BYTE:
        type = GL_UNSIGNED_BYTE;
        break;
      case IL_BYTE:
        type = GL_BYTE;
        break;
      case IL_UNSIGNED_SHORT:
        type = GL_UNSIGNED_SHORT;
        break;
      case IL_SHORT:
        type = GL_SHORT;
        break;
      case IL_UNSIGNED_INT:
        type = GL_UNSIGNED_INT;
        break;
      case IL_INT:
        type = GL_INT;
        break;
      case IL_FLOAT:
      default:
        type = GL_FLOAT;
      }
    }
  }
}