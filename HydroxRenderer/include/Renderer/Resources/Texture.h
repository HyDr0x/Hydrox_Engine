#ifndef TEXTURE_H_
#define TEXTURE_H_

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
	

    class GRAPHICAPI Texture : public ManagedResource
    {
    public:

      Texture() {}
      Texture(const Texture&);

	    Texture(GLuint width, GLuint height, GLenum target, GLenum type, GLenum internalFormat, GLenum format, void* data = NULL);
      Texture& operator=(const Texture& o);
	    ~Texture();

      void free();

	    void setTexture(GLuint slot);
	    void unsetTexture();

	    void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint magFilter, GLint minFilter);

	    util::Vector<unsigned int, 2> getResolution();
	    GLuint getSlot();
	    GLenum getTarget();

	    friend class Renderquad;
      friend class PBO;

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
