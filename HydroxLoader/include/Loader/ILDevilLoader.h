#ifndef ILDEVILLOADER_H_
#define ILDEVILLOADER_H_

#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>

#include "Loader/ShaderLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI ILDevilLoader : public ResourceLoader
    {
    public:

      ILDevilLoader(util::SingletonManager *singletonManager);
      ILDevilLoader(const ILDevilLoader& o);
      ILDevilLoader& operator=(const ILDevilLoader& o);
      ~ILDevilLoader();

      void setTarget(GLenum target);

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource();

    private:

      void getImageInformations(GLsizei& width, GLsizei& height, GLenum& internalFormat, GLenum& format, GLenum& type, GLuint& components);

      renderer::TextureManager *m_textureManager;

      GLenum m_target;
    };
  }
}

#endif
