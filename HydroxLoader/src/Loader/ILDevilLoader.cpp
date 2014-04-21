#include "Loader/ILDevilLoader.h"

#include <Renderer/Resources/Texture2D.h>

namespace he
{
  namespace loader
  {
    ILDevilLoader::ILDevilLoader(util::SingletonManager *singletonManager) : ResourceLoader(singletonManager), m_textureManager(m_singletonManager->getService<renderer::TextureManager>()), m_target(GL_TEXTURE_2D)
    {
    }

    ILDevilLoader::ILDevilLoader(const ILDevilLoader& o ) : ResourceLoader(o)
    {
      m_textureManager = o.m_textureManager;
      m_target = o.m_target;
    }

    ILDevilLoader& ILDevilLoader::operator=(const ILDevilLoader& o)
    {
      m_textureManager = o.m_textureManager;

      return *this;
    }

    ILDevilLoader::~ILDevilLoader()
    {
    }

    void ILDevilLoader::setTarget(GLenum target)
    {
      m_target = target;
    }

    util::ResourceHandle ILDevilLoader::loadResource(std::string filename)
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
		    ILboolean success = ilLoadImage(filename.c_str());

		    if(!success)
        {
          std::cout << "ERROR, couldn't open file: " << filename << std::endl;

          return getDefaultResource();
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
        
          tmpTextureID = m_textureManager->addObject(renderer::Texture2D(width, height, m_target, type, internalFormat, format, ilGetData()));
        }
      }
      ilBindImage(0);
      ilDeleteImages(1, &tex);

      return tmpTextureID;
    }

    util::ResourceHandle ILDevilLoader::getDefaultResource()
    {
      util::Vector<float, 3> textureData = util::Vector<float, 3>(0.0f, 1.0f, 0.0f);
      return m_textureManager->addObject(renderer::Texture2D(1, 1, GL_TEXTURE_2D, GL_FLOAT, GL_RGB8, GL_RGB, &textureData[0]));
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