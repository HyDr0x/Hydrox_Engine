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
    class GRAPHICAPI Texture3D : public Texture
    {
    public:

      Texture3D() {}
      Texture3D(GLuint width, GLuint height, GLuint depth, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerComponent, void* data = nullptr, bool mipmapping = false);
      Texture3D(const Texture3D& other);

      ~Texture3D();

      Texture3D& operator=(Texture3D other);

      void copyTextureData(const Texture3D& texture);

      void bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
      void unbindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

      void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint edgeModeR, GLint magFilter, GLint minFilter) const;

      void bindToFramebuffer(GLenum attachment, GLint layer);

      util::vec3ui getResolution() const;

      virtual unsigned int getTextureSize() const;

      util::SharedPointer<Texture2D> convertToTexture2D(unsigned int depth) const;

    protected:

      virtual void updateHash();

    private:

      void swap(Texture3D& other);

      GLsizei m_height;
      GLsizei m_depth;
    };
  }
}
#endif
