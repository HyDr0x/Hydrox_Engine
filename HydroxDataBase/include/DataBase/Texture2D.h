#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "DataBase/DLLExport.h"
#include "DataBase/Texture.h"

namespace he
{
  namespace db
  {
    class GRAPHICAPI Texture2D : public Texture
    {
    public:

      Texture2D() {}
      Texture2D(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerComponent, void* data = nullptr, bool mipmapping = false);
      Texture2D(const Texture2D& other);

      ~Texture2D();

      Texture2D& operator=(Texture2D other);

      void bindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format);
      void unbindImageTexture(GLuint unit, GLint level, GLenum access, GLenum format);

      void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter) const;

      void bindToFramebuffer(GLenum attachment);

      util::vec2ui getResolution() const;

      virtual unsigned int getTextureSize() const;

      void setTextureData(GLuint offsetX, GLuint offsetY, GLuint width, GLuint height, GLvoid* data);

    protected:

      virtual void updateHash();

    private:

      void swap(Texture2D& other);

    };
  }
}
#endif
