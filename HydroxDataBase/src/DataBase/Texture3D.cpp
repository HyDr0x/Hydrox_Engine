#include "DataBase/Texture3D.h"

#include <cassert>
#include <vector>

namespace he
{
  namespace db
  {
    Texture3D::Texture3D(GLuint width, GLuint height, GLuint depth, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerPixel, void* data, bool mipmapping) : 
      m_target(target),
      m_width(width),
      m_height(height),
      m_depth(depth),
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
        unsigned int length = m_width * m_height * m_depth * bytesPerPixel;
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
      glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT);
      glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type, data);

      if(m_mipmapping)
      {
        glGenerateMipmap(m_target);
      }
      glBindTexture(m_target, 0);
    }

    Texture3D::Texture3D(const Texture3D& other)
    {
      if(other.m_texIndex != 0)
      {
        m_hash = other.m_hash;
        m_width = other.m_width;
        m_height = other.m_height;
        m_depth = other.m_depth;
        m_target = other.m_target;
        m_internalFormat = other.m_internalFormat;
        m_format = other.m_format;
        m_type = other.m_type;
        m_bitsPerPixel = other.m_bitsPerPixel;
        m_channelNumber = other.m_channelNumber;
        m_mipmapping = other.m_mipmapping;

        std::vector<GLubyte> data(m_width * m_height * m_depth * (m_bitsPerPixel / 8.0f));

        other.getTextureData(&data[0]);

        glGenTextures(1, &m_texIndex);
        glBindTexture(m_target, m_texIndex);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type, &data[0]);

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

    Texture3D& Texture3D::operator=(Texture3D other)//copy swap idiom
    {
      swap(other);

      return *this;
    }

    Texture3D::~Texture3D()
    {
      glDeleteTextures(1, &m_texIndex);
    }

    void Texture3D::swap(Texture3D& other)
    {
      std::swap(m_hash, other.m_hash);
      std::swap(m_width, other.m_width);
      std::swap(m_height, other.m_height);
      std::swap(m_depth, other.m_depth);
      std::swap(m_target, other.m_target);
      std::swap(m_texIndex, other.m_texIndex);
      std::swap(m_internalFormat, other.m_internalFormat);
      std::swap(m_format, other.m_format);
      std::swap(m_type, other.m_type);
      std::swap(m_bitsPerPixel, other.m_bitsPerPixel);
      std::swap(m_channelNumber, other.m_channelNumber);
      std::swap(m_mipmapping, other.m_mipmapping);
    }

    void Texture3D::free()
    {
      glDeleteTextures(1, &m_texIndex);
    }

    void Texture3D::clearTexture(const void *data) const
    {
      glBindTexture(m_target, m_texIndex);
      glClearTexImage(m_texIndex, 0, m_format, m_type, data);
      glBindTexture(m_target, 0);
    }

    void Texture3D::bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
    {
      glBindImageTexture(unit, m_texIndex, level, layered, layer, access, format);
    }

    void Texture3D::unbindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
    {
      glBindImageTexture(unit, 0, level, layered, layer, access, format);
    }

    void Texture3D::setTexture(GLint location, GLuint slot) const
    {
      assert(slot < 31 && "ERROR, texture slot too high/n");

      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(m_target, m_texIndex);
      glUniform1i(location, slot);
    }

    void Texture3D::unsetTexture(GLuint slot) const
    {
      glActiveTexture(GL_TEXTURE0 + slot);
      glBindTexture(m_target, 0);
    }

    void Texture3D::generateMipMapps()
    {
      m_mipmapping = true;

      glBindTexture(m_target, m_texIndex);
        glGenerateMipmap(m_target);
      glBindTexture(m_target, 0);
    }

    void Texture3D::setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint edgeModeR, GLint magFilter, GLint minFilter) const
    {
      glBindTexture(m_target, m_texIndex);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, edgeModeS);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, edgeModeT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_R, edgeModeR);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
      glBindTexture(m_target, 0);
    }

    void Texture3D::bindToFramebuffer(GLenum attachment, GLint layer)
    {
      glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, m_texIndex, 0, layer);
    }

    util::vec3ui Texture3D::getResolution() const
    {
      return util::vec3ui(m_width, m_height, m_depth);
    }

    GLenum Texture3D::getTarget() const
    {
      return m_target;
    }

    GLenum Texture3D::getInternalFormat() const
    {
      return m_internalFormat;
    }

    GLenum Texture3D::getFormat() const
    {
      return m_format;
    }

    GLenum Texture3D::getType() const
    {
      return m_type;
    }

    GLuint Texture3D::getChannelNumber() const
    {
      return m_channelNumber;
    }

    GLuint Texture3D::getBitsPerPixel() const
    {
      return m_bitsPerPixel;
    }

    unsigned int Texture3D::getTextureSize() const
    {
      unsigned int bytesPerPixel = m_bitsPerPixel / 8;

      return m_width * m_height * m_depth * bytesPerPixel;
    }

    void Texture3D::getTextureData(GLvoid* data) const
    {
      glBindTexture(m_target, m_texIndex);
      glGetTexImage(m_target, 0, m_format, m_type, data);
      glBindTexture(m_target, 0);
    }

    util::SharedPointer<Texture2D> Texture3D::convertToTexture2D(unsigned int depth) const
    {
      util::SharedPointer<Texture2D> texture2D = util::SharedPointer<Texture2D>(new Texture2D(m_width, m_height, GL_TEXTURE_2D, m_type, m_internalFormat, m_format, m_channelNumber, m_bitsPerPixel));

      glCopyImageSubData(m_texIndex, m_target, 0, 0, 0, depth, texture2D->getIndex(), texture2D->getTarget(), 0, 0, 0, 0, m_width, m_height, 1);

      return texture2D;
    }
  }
}
