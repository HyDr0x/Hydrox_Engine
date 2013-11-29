#ifndef ILDEVILLOADER_H_
#define ILDEVILLOADER_H_

#include <IL/il.h>
#include <IL/ilu.h>

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Texture.h"

#include "Hydrox/Services/CacheManager.hpp"

namespace he
{
  class GRAPHICAPI ILDevilLoader
  {
  public:

    ILDevilLoader(TextureManager *textureManager);
    ILDevilLoader(const ILDevilLoader& o );
    ILDevilLoader& operator=(const ILDevilLoader& o);
    ~ILDevilLoader();

    ResourceHandle load(const char* filename, GLenum target);

  private:

    ILDevilLoader(){}

    void getImageInformations(GLsizei& width, GLsizei& height, GLenum& internalFormat, GLenum& format, GLenum& type, GLuint& components);

    TextureManager *m_textureManager;
  };
}

#endif