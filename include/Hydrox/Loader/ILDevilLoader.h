#ifndef ILDEVILLOADER_H_
#define ILDEVILLOADER_H_

#include <string>

#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Texture.h"

#include "Hydrox/Singletons/CacheManager.hpp"

namespace he
{
  class GRAPHICAPI ILDevilLoader
  {
  public:

    ILDevilLoader();
    ILDevilLoader(const ILDevilLoader& o );
    ILDevilLoader& operator=(const ILDevilLoader& o);
    ~ILDevilLoader();

    ResourceHandle load(std::string filename, GLenum target);

  private:

    void getImageInformations(GLsizei& width, GLsizei& height, GLenum& internalFormat, GLenum& format, GLenum& type, GLuint& components);

    TextureManager *m_textureManager;
  };
}

#endif
