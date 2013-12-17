#include "Hydrox/Graphics/Texture.h"

namespace he
{
  Texture::Texture(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, void* data) : m_target(target),
	                                                                                                                              m_width(width),
	                                                                                                                              m_height(height),
                                                                                                                                m_type(type),
                                                                                                                                m_internalFormat(internalFormat),
                                                                                                                                m_format(format),
                                                                                                                                m_slot(0)
  {
    unsigned int bytesPerPixel;

    switch(m_internalFormat)
    {
    case GL_RGB16:
      bytesPerPixel = 6;
      break;
    case GL_RGB32F:
      bytesPerPixel = 12;
      break;
    case GL_RGB8:
      bytesPerPixel = 3;
      break;
    case GL_RGBA16:
      bytesPerPixel = 8;
      break;
    case GL_RGBA32F:
      bytesPerPixel = 16;
      break;
    case GL_RGBA8:
    default:
      bytesPerPixel = 4;
    }

    unsigned int length = m_width * m_height * bytesPerPixel;
    m_hash = MurmurHash64A(data, length, 0);

	  glGenTextures(1, &m_texIndex);
		  glBindTexture(m_target, m_texIndex);
		  glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		  glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		  glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		  glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		  glTexImage2D(m_target, 0, internalFormat, m_width, m_height, 0, m_format, m_type, data);
	  glBindTexture(m_target, 0);
  }

  Texture::Texture(const Texture& o)
  {
    m_hash = o.m_hash;
    m_width = o.m_width;
    m_height = o.m_height;
	  m_texIndex = o.m_texIndex;
	  m_target = o.m_target;
    m_internalFormat = o.m_internalFormat;
    m_format = o.m_format;
    m_type = o.m_type;
	  m_slot = o.m_slot;
  }

  Texture& Texture::operator=(const Texture& o)
  {
    m_hash = o.m_hash;
    m_width = o.m_width;
    m_height = o.m_height;
	  m_texIndex = o.m_texIndex;
	  m_target = o.m_target;
    m_internalFormat = o.m_internalFormat;
    m_format = o.m_format;
    m_type = o.m_type;
	  m_slot = o.m_slot;

    return *this;
  }

  Texture::~Texture()
  {
  }

  void Texture::free()
  {
    glDeleteTextures(1, &m_texIndex);
  }

  void Texture::setTexture(GLuint slot)
  {
	  if(slot > 31)
    {
		  printf("ERROR, texture slot too high %i\n", slot);
    }

	  m_slot = slot;

	  glActiveTexture(GL_TEXTURE0 + m_slot);
	  glBindTexture(m_target, m_texIndex);
  }

  void Texture::unsetTexture()
  {
	  glActiveTexture(GL_TEXTURE0 + m_slot);
	  glBindTexture(m_target, 0);
  }

  void Texture::setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter)
  {
	  glBindTexture(m_target, m_texIndex);
		  glTexParameteri(m_target, GL_TEXTURE_WRAP_S, edgeModeS);
		  glTexParameteri(m_target, GL_TEXTURE_WRAP_T, edgeModeT);
		  glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);
		  glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
	  glBindTexture(m_target, 0);
  }

  Vector<unsigned int, 2> Texture::getResolution()
  {
	  return Vector<unsigned int, 2>(m_width, m_height);
  }

  GLuint Texture::getSlot()
  {
	  return m_slot;
  }

  GLenum Texture::getTarget()
  {
	  return m_target;
  }
}
