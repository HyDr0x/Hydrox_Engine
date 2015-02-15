#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "DataBase/DLLExport.h"
#include "DataBase/ManagedResource.h"

namespace he
{
  namespace db
  {
    class GRAPHICAPI Texture2D : public ManagedResource
    {
    public:

      Texture2D() : m_texIndex(0) {}
      Texture2D(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerPixel, void* data = nullptr, bool mipmapping = false);
      Texture2D(const Texture2D& other);

      ~Texture2D();

      Texture2D& operator=(Texture2D other);

      void free();

      void clearTexture(const void *data) const;

      void bindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format);
      void unbindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format);

      void setTexture(GLint location, GLuint slot) const;
      void unsetTexture(GLuint slot) const;

      void generateMipMapps();

      void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter) const;

      void bindToFramebuffer(GLenum attachment);

      util::vec2ui getResolution() const;
      GLuint getIndex() const;
      GLenum getTarget() const;
      GLenum getInternalFormat() const;
      GLenum getFormat() const;
      GLenum getType() const;
      GLuint getChannelNumber() const;
      GLuint getBitsPerPixel() const;

      unsigned int getTextureSize() const;

      void getTextureData(GLvoid* data) const;

    private:

      void swap(Texture2D& other);

      GLsizei m_width, m_height;
      GLuint m_texIndex;
      GLenum m_target;
      GLenum m_internalFormat;
      GLenum m_format;
      GLenum m_type;
      GLuint m_channelNumber;
      GLuint m_bitsPerPixel;
      bool m_mipmapping;
    };
  }
}
#endif
