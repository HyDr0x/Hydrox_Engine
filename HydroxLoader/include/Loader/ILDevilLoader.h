#ifndef ILDEVILLOADER_H_
#define ILDEVILLOADER_H_

#include <string>

#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>

#include "Loader/DLLExport.h"

#include <Renderer/Resources/ResourceManager.hpp>

namespace he
{
  namespace loader
  {
    class GRAPHICAPI ILDevilLoader
    {
    public:

      ILDevilLoader(renderer::TextureManager *textureManager);
      ILDevilLoader(const ILDevilLoader& o );
      ILDevilLoader& operator=(const ILDevilLoader& o);
      ~ILDevilLoader();

      util::ResourceHandle load(std::string path, std::string filename, GLenum target);

      util::ResourceHandle getDefaultTexture();

    private:

      void getImageInformations(GLsizei& width, GLsizei& height, GLenum& internalFormat, GLenum& format, GLenum& type, GLuint& components);

      renderer::TextureManager *m_textureManager;
    };
  }
}

#endif
