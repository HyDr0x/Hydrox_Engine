#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <list>

#include <GL/glew.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Math/Math.hpp>

#include "DataBase/ManagedResource.h"

#include "DataBase/DLLExport.h"

namespace he
{
	namespace db
	{
    class GRAPHICAPI Texture : public ManagedResource
    {
    public:

      Texture() : m_texIndex(0) {}
      Texture(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, GLuint channelNumber, GLuint bitsPerComponent, void* data, bool mipmapping = true);

      virtual ~Texture();

      void free();

      void clearTexture(const void *data);

      void setTexture(GLint location, GLuint slot) const;
      void unsetTexture(GLuint slot) const;

      void generateMipMapps();

      GLuint getIndex() const;
      GLenum getTarget() const;
      GLenum getInternalFormat() const;
      GLenum getFormat() const;
      GLenum getType() const;
      GLuint getChannelNumber() const;
      GLuint getBitsPerComponent() const;

      virtual unsigned int getTextureSize() const = 0;

      void getTextureData(GLvoid* data) const;
      void getTextureData(GLenum format, GLenum type, GLvoid* data) const;

    protected:

	    GLsizei m_width, m_height;
	    GLuint m_texIndex;
	    GLenum m_target;
      GLenum m_internalFormat;
      GLenum m_format;
      GLenum m_type;
      GLuint m_channelNumber;
      GLuint m_bitsPerComponent;
      bool m_mipmapping;
    };
	}
}
#endif
