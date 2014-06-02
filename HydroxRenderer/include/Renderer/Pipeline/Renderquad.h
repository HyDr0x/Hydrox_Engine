#ifndef RENDERQUAD_H_
#define RENDERQUAD_H_

#include <vector>
#include <iostream>
#include <stdarg.h>
#include <assert.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>

#include "Renderer/DLLExport.h"

#include "Renderer/Resources/Texture2D.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

	namespace renderer
	{
    class GRAPHICAPI Renderquad
    {
    public:

	    Renderquad();
	    ~Renderquad();

      void initialize(util::SingletonManager *singletonManager);

	    void setRenderTargets(util::SharedPointer<Texture2D> depthTexture, std::vector<util::SharedPointer<Texture2D>> textures);
	    void setRenderTargets(util::SharedPointer<Texture2D> depthTexture, int count, ...);

      void setWriteFrameBuffer() const;
      void unsetWriteFrameBuffer() const;

	    void render(util::ResourceHandle shaderHandle) const;
      void renderReadFromTextures(util::ResourceHandle shaderHandle) const;

      void clearTargets(float clearDepthValue, std::vector<util::Vector<float, 4>>& clearColors) const;

    private:

	    Renderquad(const Renderquad&);
	    Renderquad& operator=(const Renderquad&);

      util::SharedPointer<Texture2D> m_depthTexture;
      std::vector<util::SharedPointer<Texture2D>> m_textures;

      GLuint m_color_tex;
      GLuint m_rboIndex;
	    GLuint m_vboindex;
	    GLuint m_fboIndex;

      util::SingletonManager *m_singletonManager;
    };
	}
}

#endif
