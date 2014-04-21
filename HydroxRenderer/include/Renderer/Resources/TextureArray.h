#ifndef TEXTUREARRAY_H_
#define TEXTUREARRAY_H_

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
    class GRAPHICAPI TextureArray : public ManagedResource
    {
    public:

      TextureArray() {}
      TextureArray(const TextureArray&);

	    TextureArray(GLuint width, GLuint height, GLuint depth, GLenum target, GLenum type, GLenum internalFormat, GLenum format, void* data = NULL, bool mipmapping = true);
      TextureArray& operator=(const TextureArray& o);
	    ~TextureArray();

      void free();

	    void setTexture(GLint location, GLuint slot);
	    void unsetTexture();

      void activateMipMapping(bool mipmapping);

      void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint edgeModeR, GLint magFilter, GLint minFilter);

	    util::Vector<unsigned int, 3> getResolution();
	    GLenum getTarget();
      GLenum getInternalFormat();
      GLenum getFormat();
      GLenum getType();

      unsigned int getTextureSize();

      void getTextureData(GLvoid* data);

	    friend class Renderquad;

    private:

	    GLsizei m_width, m_height, m_depth;
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
