#ifndef TEXTURE1D_H_
#define TEXTURE1D_H_

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "DataBase/DLLExport.h"
#include "DataBase/Texture.h"

namespace he
{
  namespace db
  {
    class GRAPHICAPI Texture1D : public Texture
    {
    public:

      Texture1D() {}
      Texture1D(GLuint width, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerComponent, void* data = nullptr, bool mipmapping = false);
      Texture1D(const Texture1D& other);

      ~Texture1D();

      Texture1D& operator=(Texture1D other);

      void copyTextureData(const Texture1D& texture);

      void bindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format);
      void unbindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format);

      void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter) const;

      void bindToFramebuffer(GLenum attachment);

      unsigned int getResolution() const;

      virtual unsigned int getTextureSize() const;

      void setTextureData(GLuint offsetX, GLuint width, GLvoid* data);

    protected:

      virtual void updateHash();

    private:

      void swap(Texture1D& other);

    };
  }
}
#endif
