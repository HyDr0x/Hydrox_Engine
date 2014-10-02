#ifndef TEXTURE3D_H_
#define TEXTURE3D_H_

#include <GL/glew.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>
#include <Utilities/Math/Math.hpp>

#include "DataBase/DLLExport.h"
#include "DataBase/ManagedResource.h"
#include "DataBase/Texture2D.h"

namespace he
{
  namespace db
  {
    class GRAPHICAPI Texture3D : public ManagedResource
    {
    public:

      Texture3D(){}
      Texture3D(GLuint width, GLuint height, GLuint depth, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerPixel, void* data = nullptr, bool mipmapping = true);
      Texture3D(const Texture3D&);

      ~Texture3D();

      Texture3D& operator=(const Texture3D& other);

      void free();

      void setTexture(GLint location, GLuint slot);
      void unsetTexture() const;

      void generateMipMapps() const;

      void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint edgeModeR, GLint magFilter, GLint minFilter) const;

      void bindToFramebuffer(GLenum attachment, GLint layer);

      util::Vector<unsigned int, 3> getResolution() const;
      GLenum getTarget() const;
      GLenum getInternalFormat() const;
      GLenum getFormat() const;
      GLenum getType() const;
      GLuint getChannelNumber() const;
      GLuint getBitsPerPixel() const;

      unsigned int getTextureSize() const;

      void getTextureData(GLvoid* data) const;

      util::SharedPointer<Texture2D> convertToTexture2D(unsigned int depth) const;

    private:

      GLsizei m_width, m_height, m_depth;
      GLuint m_texIndex;
      GLenum m_target;
      GLenum m_internalFormat;
      GLenum m_format;
      GLenum m_type;
      GLuint m_slot;
      GLuint m_channelNumber;
      GLuint m_bitsPerPixel;
    };
  }
}
#endif
