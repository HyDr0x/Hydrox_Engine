#include "DataBase/Texture2D.h"

#include <cassert>
#include <vector>

namespace he
{
  namespace db
  {
    Texture2D::Texture2D(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerPixel, void* data, bool mipmapping) : m_target(target),
      m_width(width),
      m_height(height),
      m_type(type),
      m_internalFormat(internalFormat),
      m_format(format),
      m_channelNumber(channelNumber),
      m_bitsPerPixel(bitsPerPixel),
      m_mipmapping(mipmapping)
    {
      if(data != nullptr)
      {
        unsigned int bytesPerPixel = bitsPerPixel / 8;
        unsigned int length = m_width * m_height * bytesPerPixel;
        m_hash = MurmurHash64A(data, length, 0);
      }
      else
      {
        m_hash = 0;
      }

      glGenTextures(1, &m_texIndex);
      glBindTexture(m_target, m_texIndex);
      glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, data);

      if(m_mipmapping)
      {
        glGenerateMipmap(m_target);
      }
      glBindTexture(m_target, 0);
    }

    Texture2D::Texture2D(const Texture2D& other)
    {
      if(other.m_texIndex != 0)
      {
        m_hash = other.m_hash;
        m_width = other.m_width;
        m_height = other.m_height;
        m_target = other.m_target;
        m_internalFormat = other.m_internalFormat;
        m_format = other.m_format;
        m_type = other.m_type;
        m_bitsPerPixel = other.m_bitsPerPixel;
        m_channelNumber = other.m_channelNumber;
        m_mipmapping = other.m_mipmapping;

        std::vector<GLubyte> data(m_width * m_height * (m_bitsPerPixel / 8.0f));

        other.getTextureData(&data[0]);

        glGenTextures(1, &m_texIndex);
        glBindTexture(m_target, m_texIndex);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, &data[0]);

        if(m_mipmapping)
        {
          glGenerateMipmap(m_target);
        }
        glBindTexture(m_target, 0);
      }
      else
      {
        m_texIndex = other.m_texIndex;
      }
    }

    Texture2D::~Texture2D()
    {
      glDeleteTextures(1, &m_texIndex);
    }

    Texture2D& Texture2D::operator=(Texture2D other)//copy swap idiom
    {
      swap(other);

      return *this;
    }

    void Texture2D::swap(Texture2D& other)
    {
      std::swap(m_hash, other.m_hash);
      std::swap(m_width, other.m_width);
      std::swap(m_height, other.m_height);
      std::swap(m_target, other.m_target);
      std::swap(m_texIndex, other.m_texIndex);
      std::swap(m_internalFormat, other.m_internalFormat);
      std::swap(m_format, other.m_format);
      std::swap(m_type, other.m_type);
      std::swap(m_bitsPerPixel, other.m_bitsPerPixel);
      std::swap(m_channelNumber, other.m_channelNumber);
      std::swap(m_mipmapping, other.m_mipmapping);
    }

    void Texture2D::free()
    {
      glDeleteTextures(1, &m_texIndex);
    }

    void Texture2D::clearTexture(const void *data) const
    {
      glBindTexture(m_target, m_texIndex);
      glClearTexImage(m_texIndex, 0, m_format, m_type, data);
      glBindTexture(m_target, 0);
    }

    void Texture2D::bindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format)
    {
      glBindImageTexture(unit, m_texIndex, level, GL_FALSE, 0, access, format);
    }

    void Texture2D::unbindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format)
    {
      glBindImageTexture(unit, 0, level, GL_FALSE, 0, access, format);
    }

    void Texture2D::setTexture(GLint location, GLuint slot) const
    {
      assert(slot < 31 && "ERROR, texture slot too high/n");

      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(m_target, m_texIndex);
      glUniform1i(location, slot);
    }

    void Texture2D::unsetTexture(GLuint slot) const
    {
      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(m_target, 0);
    }

    void Texture2D::generateMipMapps()
    {
      m_mipmapping = true;
      glBindTexture(m_target, m_texIndex);
        glGenerateMipmap(m_target);
      glBindTexture(m_target, 0);
    }

    void Texture2D::setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter) const
    {
      glBindTexture(m_target, m_texIndex);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, edgeModeS);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, edgeModeT);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
      glBindTexture(m_target, 0);
    }

    void Texture2D::bindToFramebuffer(GLenum attachment)
    {
      glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, m_target, m_texIndex, 0);
    }

    util::vec2ui Texture2D::getResolution() const
    {
      return util::vec2ui(m_width, m_height);
    }

    GLuint Texture2D::getIndex() const
    {
      return m_texIndex;
    }

    GLenum Texture2D::getTarget() const
    {
      return m_target;
    }

    GLenum Texture2D::getInternalFormat() const
    {
      return m_internalFormat;
    }

    GLenum Texture2D::getFormat() const
    {
      return m_format;
    }

    GLenum Texture2D::getType() const
    {
      return m_type;
    }

    GLuint Texture2D::getChannelNumber() const
    {
      return m_channelNumber;
    }

    GLuint Texture2D::getBitsPerPixel() const
    {
      return m_bitsPerPixel;
    }

    unsigned int Texture2D::getTextureSize() const
    {
      unsigned int bytesPerPixel= m_bitsPerPixel / 8;

      return m_width * m_height * bytesPerPixel;
    }

    void Texture2D::getTextureData(GLvoid* data) const
    {
      glBindTexture(m_target, m_texIndex);
      glGetTexImage(m_target, 0, m_format, m_type, data);
      glBindTexture(m_target, 0);
    }
  }
}
