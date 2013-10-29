#include "Hydrox/Loader/ILDevilLoader.h"


ILDevilLoader::ILDevilLoader(TextureManager *textureManager) : m_textureManager(textureManager)
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

ResourceHandle ILDevilLoader::load(const char* filename, GLenum target)
{
  ResourceHandle tmpTextureID;
  GLsizei width, height;
  GLenum internalFormat;
  GLenum format;
  GLenum type;

  ILuint tex;
	ilGenImages(1, &tex);
	ilBindImage(tex);
  {
    std::string texturePath = m_textureManager->getPath();
    texturePath += std::string(filename);
		ILboolean success = ilLoadImage((const wchar_t*)(texturePath).c_str());

		if(!success)
    {
			printf("ERROR, couldn't open file %s\n", filename);
    }

    GLuint componentNumber = 0;
    getImageInformations(width, height, internalFormat, format, type, componentNumber);

  //  success = ilConvertImage(format, type);
		//if(!success)
  //  {
		//	printf("ERROR, couldn't convert file %s\n", filename);
  //  }

    tmpTextureID = m_textureManager->addObject(Texture(width, height, target, type, internalFormat, format, ilGetData()));
  }
  ilBindImage(0);
  ilDeleteImages(1, &tex);

  return tmpTextureID;
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
    case 16:
      internalFormat = GL_RGB16;
      break;
    case 32:
      internalFormat = GL_RGB32F;
      break;
    case 8:
    default:
      internalFormat = GL_RGB8;
    }
    break;
  case GL_RGBA:
  case GL_BGRA:
    components = 4;
    switch(ilGetInteger(IL_IMAGE_BITS_PER_PIXEL))
    {
    case 16:
      internalFormat = GL_RGBA16;
      break;
    case 32:
      internalFormat = GL_RGBA32F;
      break;
    case 8:
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
