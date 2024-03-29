#include "Renderer/Resources/TextureArray.h"

#include <assert.h>

namespace he
{
	namespace renderer
	{
	

    TextureArray::TextureArray(GLuint width, GLuint height, GLuint depth, GLenum target, GLenum type, GLenum internalFormat, GLenum format, void* data, bool mipmapping) : m_target(target),
      m_width(width),
      m_height(height),
      m_depth(depth),
      m_type(type),
      m_internalFormat(internalFormat),
      m_format(format),
      m_slot(0)
    {
      unsigned int bytesPerPixel;
      unsigned int components = 0;

      switch (m_internalFormat)
      {
      case GL_RGB16:
        bytesPerPixel = 6;
        components = 3;
        break;
      case GL_RGB32F:
        bytesPerPixel = 12;
        components = 3;
        break;
      case GL_RGB8:
        bytesPerPixel = 3;
        components = 3;
        break;
      case GL_RGBA16:
        bytesPerPixel = 8;
        components = 4;
        break;
      case GL_RGBA32F:
        bytesPerPixel = 16;
        components = 4;
        break;
      case GL_RGBA8:
      default:
        bytesPerPixel = 4;
        components = 4;
      }

      unsigned int length = m_width * m_height * m_depth * bytesPerPixel;
      m_hash = MurmurHash64A(data, length, 0);

      glGenTextures(1, &m_texIndex);
      glBindTexture(m_target, m_texIndex);
      glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT);
      glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type, data);

      if (mipmapping)
      {
        glGenerateMipmap(m_target);
      }
	    glBindTexture(m_target, 0);
    }

    TextureArray::TextureArray(const TextureArray& o)
    {
      m_hash = o.m_hash;
      m_width = o.m_width;
      m_depth = o.m_depth;
      m_height = o.m_height;
	    m_texIndex = o.m_texIndex;
	    m_target = o.m_target;
      m_internalFormat = o.m_internalFormat;
      m_format = o.m_format;
      m_type = o.m_type;
	    m_slot = o.m_slot;
    }

    TextureArray& TextureArray::operator=(const TextureArray& o)
    {
      m_hash = o.m_hash;
      m_width = o.m_width;
      m_height = o.m_height;
      m_depth = o.m_depth;
	    m_texIndex = o.m_texIndex;
	    m_target = o.m_target;
      m_internalFormat = o.m_internalFormat;
      m_format = o.m_format;
      m_type = o.m_type;
	    m_slot = o.m_slot;

      return *this;
    }

    TextureArray::~TextureArray()
    {
    }

    void TextureArray::free()
    {
      glDeleteTextures(1, &m_texIndex);
    }

    void TextureArray::setTexture(GLint location, GLuint slot)
    {
      assert(slot < 31 && "ERROR, texture slot too high\n");

	    m_slot = slot;

	    glActiveTexture(GL_TEXTURE0 + m_slot);
	    glBindTexture(m_target, m_texIndex);
      glUniform1i(location, slot);
    }

    void TextureArray::unsetTexture()
    {
	    glActiveTexture(GL_TEXTURE0 + m_slot);
	    glBindTexture(m_target, 0);
    }

    void TextureArray::setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint edgeModeR, GLint magFilter, GLint minFilter)
    {
	    glBindTexture(m_target, m_texIndex);
		    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, edgeModeS);
		    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, edgeModeT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_R, edgeModeR);
		    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);
		    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
	    glBindTexture(m_target, 0);
    }

    util::Vector<unsigned int, 3> TextureArray::getResolution()
    {
	    return util::Vector<unsigned int, 3>(m_width, m_height, m_depth);
    }

    GLenum TextureArray::getTarget()
    {
	    return m_target;
    }

    GLenum TextureArray::getInternalFormat()
    {
      return m_internalFormat;
    }

    GLenum TextureArray::getFormat()
    {
      return m_format;
    }

    GLenum TextureArray::getType()
    {
      return m_type;
    }

    unsigned int TextureArray::getTextureSize()
    {
      unsigned int bytesPerPixel;

      switch (m_internalFormat)
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

      return m_width * m_height * m_depth * bytesPerPixel;
    }

    void TextureArray::getTextureData(GLvoid* data)
    {
      glBindTexture(m_target, m_texIndex);
      glGetTexImage(m_target, 0, m_format, m_type, data);
      glBindTexture(m_target, 0);
    }
	}
}
