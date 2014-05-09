#ifndef TEXTURE3D_H_
#define TEXTURE3D_H_

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
    class GRAPHICAPI Texture3D : public ManagedResource
    {
    public:

      Texture3D() {}
	    Texture3D(GLuint width, GLuint height, GLuint depth, GLenum target, GLenum type, GLenum internalFormat, GLenum format, void* data = nullptr, bool mipmapping = true);
      Texture3D(const Texture3D&);

	    ~Texture3D();

      Texture3D& operator=(const Texture3D& o);

      void free();

	    void setTexture(GLint location, GLuint slot);
	    void unsetTexture() const;

      void generateMipMapps() const;

      void setTexParameters(GLint edgeModeS, GLint edgeModeT, GLint edgeModeR, GLint magFilter, GLint minFilter) const;

	    util::Vector<unsigned int, 3> getResolution() const;
	    GLenum getTarget() const;
      GLenum getInternalFormat() const;
      GLenum getFormat() const;
      GLenum getType() const;

      unsigned int getTextureSize() const;

      void getTextureData(GLvoid* data) const;

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
