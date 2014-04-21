#ifndef TEXTURE2D_H_
#define TEXTURE2D_H_

#include <list>

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

#include <Utilities/Math/Math.hpp>

#include "Renderer/Resources/ManagedResource.h"
#include <Utilities/Miscellaneous/ResourceHandle.h>

namespace he
{
	namespace renderer
	{
    class GRAPHICAPI Texture2D : public ManagedResource
    {
    public:

      Texture2D() {}
      Texture2D(const Texture2D&);

	    Texture2D(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, void* data = NULL, bool mipmapping = true);
      Texture2D& operator=(const Texture2D& o);
	    ~Texture2D();

      void free();

	    void setTexture(GLint location, GLuint slot);
	    void unsetTexture();

      void activateMipMapping(bool mipmapping);

	    void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter);

	    util::Vector<unsigned int, 2> getResolution();
	    GLenum getTarget();
      GLenum getInternalFormat();
      GLenum getFormat();
      GLenum getType();

      unsigned int getTextureSize();

      void getTextureData(GLvoid* data);

	    friend class Renderquad;

    private:

	    GLsizei m_width, m_height;
	    GLuint m_texIndex;
	    GLenum m_target;
      GLenum m_internalFormat;
      GLenum m_format;
      GLenum m_type;
	    GLuint m_slot;
    };
	}
}
#endif
