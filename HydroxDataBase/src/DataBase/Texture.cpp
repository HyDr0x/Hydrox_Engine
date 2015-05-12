#include "DataBase/Texture.h"

#include <cassert>
#include <vector>

namespace he
{
  namespace db
	{
    Texture::Texture(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerComponent, void* data, bool mipmapping) : 
      m_target(target),
      m_width(width),
      m_height(height),
      m_type(type),
      m_internalFormat(internalFormat),
      m_format(format),
      m_channelNumber(channelNumber),
      m_bitsPerComponent(bitsPerComponent),
      m_mipmapping(mipmapping)
    {
    }

    Texture::~Texture()
    {
      glDeleteTextures(1, &m_texIndex);
    }

    void Texture::free()
    {
      glDeleteTextures(1, &m_texIndex);
    }

    void Texture::clearTexture(const void *data)
    {
      glBindTexture(m_target, m_texIndex);
      glClearTexImage(m_texIndex, 0, m_format, m_type, data);
      glBindTexture(m_target, 0);

      m_dirtyHash = true;
    }

    void Texture::setTexture(GLint location, GLuint slot) const
    {
      assert(slot < 31 && "ERROR, texture slot too high/n");

      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(m_target, m_texIndex);
      glUniform1i(location, slot);
    }

    void Texture::unsetTexture(GLuint slot) const
    {
      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(m_target, 0);
    }

    void Texture::generateMipMapps()
    {
      m_mipmapping = true;
      glBindTexture(m_target, m_texIndex);
      glGenerateMipmap(m_target);
      glBindTexture(m_target, 0);
    }

    GLuint Texture::getIndex() const
    {
      return m_texIndex;
    }

    GLenum Texture::getTarget() const
    {
      return m_target;
    }

    GLenum Texture::getInternalFormat() const
    {
      return m_internalFormat;
    }

    GLenum Texture::getFormat() const
    {
      return m_format;
    }

    GLenum Texture::getType() const
    {
      return m_type;
    }

    GLuint Texture::getChannelNumber() const
    {
      return m_channelNumber;
    }

    GLuint Texture::getBitsPerComponent() const
    {
      return m_bitsPerComponent;
    }

    void Texture::getTextureData(GLvoid* data) const
    {
      glBindTexture(m_target, m_texIndex);
      glGetTexImage(m_target, 0, m_format, m_type, data);
      glBindTexture(m_target, 0);
    }

    void Texture::getTextureData(GLenum format, GLenum type, GLvoid* data) const
    {
      glBindTexture(m_target, m_texIndex);
      glGetTexImage(m_target, 0, format, type, data);
      glBindTexture(m_target, 0);
    }
	}
}
