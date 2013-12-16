#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <list>

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Utility/Miscellaneous/CachedResource.h"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

namespace he
{
  class GRAPHICAPI Texture : public CachedResource
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

	  Vector<unsigned int, 2> getResolution();
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
#endif