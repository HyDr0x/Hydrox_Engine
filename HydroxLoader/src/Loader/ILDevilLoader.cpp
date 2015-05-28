#include "Loader/ILDevilLoader.h"

#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Texture2D.h>

namespace he
{
  namespace loader
  {
    ILDevilLoader::ILDevilLoader(util::SingletonManager *singletonManager) : ResourceLoader(singletonManager), m_textureManager(m_singletonManager->getService<db::TextureManager>()), m_target(GL_TEXTURE_2D)
    {
    }

    ILDevilLoader::ILDevilLoader(const ILDevilLoader& other ) : ResourceLoader(other)
    {
      m_textureManager = other.m_textureManager;
      m_target = other.m_target;
    }

    ILDevilLoader::~ILDevilLoader()
    {
    }

    ILDevilLoader& ILDevilLoader::operator=(const ILDevilLoader& other)
    {
      m_textureManager = other.m_textureManager;

      return *this;
    }

    void ILDevilLoader::setTarget(GLenum target)
    {
      m_target = target;
    }

    void ILDevilLoader::setSRGB(bool srgb)
    {
      m_srgb = srgb;
    }

    void ILDevilLoader::setMipMapping(bool mipmapping)
    {
      m_mipmapping = mipmapping;
    }

    util::ResourceHandle ILDevilLoader::loadResource(std::string filename)
    {
      util::ResourceHandle tmpTextureID;
      GLsizei width, height;
      GLenum internalFormat;
      GLenum format;
      GLenum type;
      GLuint channelNumber;
      GLuint bitsPerComponent;

      ilEnable(IL_ORIGIN_SET);
      ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

      ILuint tex;
      ilGenImages(1, &tex);
      ilBindImage(tex);
      {
        ILboolean success = ilLoadImage(filename.c_str());

        if(!success)
        {
          std::clog << "ERROR, couldn't open file: " << filename << std::endl;

          return getDefaultResource();
        }
        else
        {
          getImageInformations(width, height, internalFormat, format, type, channelNumber, bitsPerComponent);

          //success = ilConvertImage(format, type);
          //if(!success)
          //{
          //  printf("ERROR, couldn't convert file %s/n", filename);
          //}
        
          tmpTextureID = m_textureManager->addObject(db::Texture2D(width, height, m_target, type, internalFormat, format, channelNumber, bitsPerComponent, ilGetData(), m_mipmapping));
        }
      }
      ilBindImage(0);
      ilDeleteImages(1, &tex);

      return tmpTextureID;
    }

    util::ResourceHandle ILDevilLoader::getDefaultResource() const
    {
      util::vec3f textureData = util::vec3f(0.0f, 1.0f, 0.0f);
      return m_textureManager->addObject(db::Texture2D(1, 1, GL_TEXTURE_2D, GL_FLOAT, GL_RGB8, GL_RGB, 3, 24, &textureData[0], true));
    }

    void ILDevilLoader::getImageInformations(GLsizei& width, GLsizei& height, GLenum& internalFormat, GLenum& format, GLenum& type, GLuint& channelNumber, GLuint& bitsPerComponent)
    {
      width = ilGetInteger(IL_IMAGE_WIDTH);
      height = ilGetInteger(IL_IMAGE_HEIGHT);
      GLuint bitsPerPixel = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);

      switch(ilGetInteger(IL_IMAGE_FORMAT))
      {
      case IL_LUMINANCE:
        format = GL_LUMINANCE;
        break;
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
      case GL_LUMINANCE:
        channelNumber = 1;
        switch(bitsPerPixel)
        {
        case 4:
          internalFormat = GL_LUMINANCE4;
          break;
        case 12:
          internalFormat = GL_LUMINANCE12;
          break;
        case 16:
          internalFormat = GL_LUMINANCE16;
          break;
        case 32:
          internalFormat = GL_LUMINANCE32F_ARB;
          break;
        case 8:
        default:
          internalFormat = GL_LUMINANCE8;
          break;
        }
        
        break;
      case GL_RGB:
      case GL_BGR:
        channelNumber = 3;
        
        switch(bitsPerPixel)
        {
        case 48:
          internalFormat = GL_RGB16F;
          break;
        case 96:
          internalFormat = GL_RGB32F;
          break;
        case 24:
        default:
          if(m_srgb)
          {
            internalFormat = GL_SRGB8;
          }
          else
          {
            internalFormat = GL_RGB8;
          }
        }
        break;
      case GL_RGBA:
      case GL_BGRA:
        channelNumber = 4;

        switch(bitsPerPixel)
        {
        case 64:
          internalFormat = GL_RGBA16F;
          break;
        case 128:
          internalFormat = GL_RGBA32F;
          break;
        case 32:
        default:
          if(m_srgb)
          {
            internalFormat = GL_SRGB8_ALPHA8;
          }
          else
          {
            internalFormat = GL_RGBA8;
          }
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
      case IL_DOUBLE:
        type = GL_DOUBLE;
        break;
      case IL_FLOAT:
      default:
        type = GL_FLOAT;
      }

      bitsPerComponent = bitsPerPixel / channelNumber;
    }
  }
}