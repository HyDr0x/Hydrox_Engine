#include "DataBase/Texture3D.h"

#include <cassert>
#include <vector>

namespace he
{
  namespace db
  {
    Texture3D::Texture3D(GLuint width, GLuint height, GLuint depth, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerComponent, void* data, bool mipmapping) :
      Texture(width, target, type, internalFormat, format, channelNumber, bitsPerComponent, data, mipmapping),
      m_height(height),
      m_depth(depth)
    {
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
        m_width = other.m_width;
        m_height = other.m_height;
        m_depth = other.m_depth;
        m_target = other.m_target;
        m_internalFormat = other.m_internalFormat;
        m_format = other.m_format;
        m_type = other.m_type;
        m_bitsPerComponent = other.m_bitsPerComponent;
        m_channelNumber = other.m_channelNumber;
        m_mipmapping = other.m_mipmapping;

        //unsigned int bytesPerComponent = (m_bitsPerComponent / 8.0f);

        //GLint byteRowAlignement;
        //glGetIntegerv(GL_PACK_ALIGNMENT, &byteRowAlignement);
        //GLsizei dataWidth = m_width * bytesPerComponent * m_channelNumber;
        //GLsizei paddedWidth = dataWidth + ((byteRowAlignement - (dataWidth % byteRowAlignement)) % byteRowAlignement);
        //std::vector<GLubyte> data(paddedWidth * m_height * m_depth);

        //other.getTextureData(&data[0]);

        //std::vector<GLubyte> textureData(m_width * m_height * m_depth * bytesPerComponent * m_channelNumber);

        //if(m_channelNumber == 4)
        //{
        //  textureData = data;
        //}
        //else
        //{
        //  for(unsigned int i = 0; i < data.size(); i++)
        //  {
        //    if(i % paddedWidth < m_width * bytesPerComponent * m_channelNumber)
        //    {
        //      textureData[i] = data[i];
        //    }
        //  }
        //}

        //glGenTextures(1, &m_texIndex);
        //glBindTexture(m_target, m_texIndex);
        //glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT);
        //glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        //glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type, &textureData[0]);

        //if(m_mipmapping)
        //{
        //  glGenerateMipmap(m_target);
        //}
        //glBindTexture(m_target, 0);

        glGenTextures(1, &m_texIndex);
        glBindTexture(m_target, m_texIndex);

        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type, nullptr);

        glBindTexture(m_target, 0);

        glCopyImageSubData(other.m_texIndex, other.m_target, 0, 0, 0, 0, m_texIndex, m_target, 0, 0, 0, 0, other.m_width, other.m_height, other.m_depth);

        if(m_mipmapping)
        {
          glBindTexture(m_target, m_texIndex);
          glGenerateMipmap(m_target);
          glBindTexture(m_target, 0);
        }
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
      std::swap(m_bitsPerComponent, other.m_bitsPerComponent);
      std::swap(m_channelNumber, other.m_channelNumber);
      std::swap(m_mipmapping, other.m_mipmapping);
    }

    void Texture3D::copyTextureData(const Texture3D& texture)
    {
      //glBindTexture(m_target, m_texIndex);

      //glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
      //glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
      //glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT);
      //glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      //glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      //glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type, nullptr);

      //glBindTexture(m_target, 0);

      glCopyImageSubData(texture.m_texIndex, texture.m_target, 0, 0, 0, 0, m_texIndex, m_target, 0, 0, 0, 0, texture.m_width, texture.m_height, texture.m_depth);

      if(m_mipmapping)
      {
        glBindTexture(m_target, m_texIndex);
        glGenerateMipmap(m_target);
        glBindTexture(m_target, 0);
      }
    }

    void Texture3D::copyTextureData(const Texture3D& texture, util::vec3i srcCoords, util::vec3i dstCoords, util::vec3i size)
    {
      glCopyImageSubData(texture.m_texIndex, texture.m_target, 0, srcCoords[0], srcCoords[1], srcCoords[2], m_texIndex, m_target, 0, dstCoords[0], dstCoords[1], dstCoords[2], size[0], size[1], size[2]);

      if(m_mipmapping)
      {
        glBindTexture(m_target, m_texIndex);
        glGenerateMipmap(m_target);
        glBindTexture(m_target, 0);
      }
    }

    void Texture3D::bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
    {
      glBindImageTexture(unit, m_texIndex, level, layered, layer, access, format);
    }

    void Texture3D::unbindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
    {
      glBindImageTexture(unit, 0, level, layered, layer, access, format);
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

    unsigned int Texture3D::getTextureSize() const
    {
      unsigned int bytesPerComponent = m_bitsPerComponent / 8;

      return m_width * m_height * m_depth * bytesPerComponent * m_channelNumber;
    }

    util::SharedPointer<Texture2D> Texture3D::convertToTexture2D(unsigned int depth) const
    {
      util::SharedPointer<Texture2D> texture2D = util::SharedPointer<Texture2D>(new Texture2D(m_width, m_height, GL_TEXTURE_2D, m_type, m_internalFormat, m_format, m_channelNumber, m_bitsPerComponent));

      glCopyImageSubData(m_texIndex, m_target, 0, 0, 0, depth, texture2D->getIndex(), texture2D->getTarget(), 0, 0, 0, 0, m_width, m_height, 1);

      return texture2D;
    }

    void Texture3D::updateHash()
    {
      if(m_width * m_height * m_depth > 0)
      {
        GLint byteRowAlignement;
        glGetIntegerv(GL_PACK_ALIGNMENT, &byteRowAlignement);

        unsigned int bytesPerComponent = m_bitsPerComponent / 8;
        GLsizei dataWidth = m_width * bytesPerComponent * m_channelNumber;
        GLsizei paddedWidth = dataWidth + ((byteRowAlignement - (dataWidth % byteRowAlignement)) % byteRowAlignement);
        std::vector<GLubyte> data(paddedWidth * m_height * m_depth);

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
