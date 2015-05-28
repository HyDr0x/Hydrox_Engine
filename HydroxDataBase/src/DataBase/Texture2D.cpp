#include "DataBase/Texture2D.h"

#include <cassert>
#include <vector>

namespace he
{
  namespace db
  {
    Texture2D::Texture2D(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerComponent, void* data, bool mipmapping) :
      Texture(width, height, target, type, internalFormat, format, channelNumber, bitsPerComponent, data, mipmapping)
    {
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
        m_width = other.m_width;
        m_height = other.m_height;
        m_target = other.m_target;
        m_internalFormat = other.m_internalFormat;
        m_format = other.m_format;
        m_type = other.m_type;
        m_bitsPerComponent = other.m_bitsPerComponent;
        m_channelNumber = other.m_channelNumber;
        m_mipmapping = other.m_mipmapping;

        /*unsigned int bytesPerComponent = (m_bitsPerComponent / 8.0f);

        GLint byteRowAlignement;
        glGetIntegerv(GL_PACK_ALIGNMENT, &byteRowAlignement);
        GLsizei dataWidth = m_width * bytesPerComponent * m_channelNumber;
        GLsizei paddedWidth = dataWidth + ((byteRowAlignement - (dataWidth % byteRowAlignement)) % byteRowAlignement);
        std::vector<GLubyte> data(paddedWidth * m_height);

        other.getTextureData(&data[0]);

        std::vector<GLubyte> textureData(m_width * m_height * bytesPerComponent * m_channelNumber);

        if(m_channelNumber == 4)
        {
          textureData = data;
        }
        else
        {
          for(unsigned int i = 0; i < data.size(); i++)
          {
            if(i % paddedWidth < m_width * bytesPerComponent * m_channelNumber)
            {
              textureData[i] = data[i];
            }
          }
        }

        glGenTextures(1, &m_texIndex);
        glBindTexture(m_target, m_texIndex);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, &textureData[0]);

        if(m_mipmapping)
        {
          glGenerateMipmap(m_target);
        }
        glBindTexture(m_target, 0);*/

        glGenTextures(1, &m_texIndex);
        glBindTexture(m_target, m_texIndex);

        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, nullptr);

        glBindTexture(m_target, 0);

        glCopyImageSubData(other.m_texIndex, other.m_target, 0, 0, 0, 0, m_texIndex, m_target, 0, 0, 0, 0, other.m_width, other.m_height, 1);

        if(m_mipmapping)
        {
          glBindTexture(m_target, m_texIndex);
          glGenerateMipmap(m_target);
          glBindTexture(m_target, 0);
        }
      }
      else
      {
        m_texIndex = 0;
      }
    }

    Texture2D::~Texture2D()
    {
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
      std::swap(m_bitsPerComponent, other.m_bitsPerComponent);
      std::swap(m_channelNumber, other.m_channelNumber);
      std::swap(m_mipmapping, other.m_mipmapping);
    }

    void Texture2D::bindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format)
    {
      glBindImageTexture(unit, m_texIndex, level, GL_FALSE, 0, access, format);
    }

    void Texture2D::unbindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format)
    {
      glBindImageTexture(unit, 0, level, GL_FALSE, 0, access, format);
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

    unsigned int Texture2D::getTextureSize() const
    {
      return m_width * m_height * (m_bitsPerComponent / 8) * m_channelNumber;
    }

    void Texture2D::setTextureData(GLuint offsetX, GLuint offsetY, GLuint width, GLuint height, GLvoid* data)
    {
      GLint byteRowAlignement;
      glGetIntegerv(GL_UNPACK_ALIGNMENT, &byteRowAlignement);
      unsigned int bytesPerComponent = m_bitsPerComponent / 8;
      GLsizei dataWidth = m_width * bytesPerComponent * m_channelNumber;


      GLint rowStride = m_width - width;
      //glPixelStorei(GL_UNPACK_ROW_LENGTH, rowStride);
      glBindTexture(m_target, m_texIndex);
      glTexSubImage2D(m_target, 0, offsetX, offsetY, width, height, m_format, m_type, data);
      glBindTexture(m_target, 0);
      //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    void Texture2D::updateHash()
    {
      if(m_width * m_height > 0)
      {
        GLint byteRowAlignement;
        glGetIntegerv(GL_PACK_ALIGNMENT, &byteRowAlignement);

        unsigned int bytesPerComponent = m_bitsPerComponent / 8;
        GLsizei dataWidth = m_width * bytesPerComponent * m_channelNumber;
        GLsizei paddedWidth = dataWidth + ((byteRowAlignement - (dataWidth % byteRowAlignement)) % byteRowAlignement);
        std::vector<GLubyte> data(paddedWidth * m_height);

        glBindTexture(m_target, m_texIndex);
        glGetTexImage(m_target, 0, m_format, m_type, &data[0]);
        glBindTexture(m_target, 0);

        m_hash = MurmurHash64A(&data[0], data.size(), 0);
      }
      else
      {
        m_hash = 0;
      }
    }
  }
}
