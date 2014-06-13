#ifndef RENDERQUAD_H_
#define RENDERQUAD_H_

#include <vector>
#include <iostream>
#include <stdarg.h>
#include <assert.h>

#include "Renderer/DLLExport.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/Texture2D.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI Renderquad
    {
    public:

      Renderquad();
      ~Renderquad();

      void setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, std::vector<util::SharedPointer<db::Texture2D>> textures);
      void setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, int count, ...);

      void setRenderTargets(RenderOptions options, std::vector<util::SharedPointer<db::Texture2D>> textures);
      void setRenderTargets(RenderOptions options, int count, ...);

      void setReadTextures(std::vector<util::SharedPointer<db::Texture2D>> textures);
      void setReadTextures(int count, ...);

      void setWriteFrameBuffer() const;
      void unsetWriteFrameBuffer() const;

      void render() const;

      void clearTargets(GLfloat clearDepthValue, std::vector<util::Vector<float, 4>>& clearColors) const;
      void clearTargets(GLfloat clearDepthValue, util::Vector<float, 4> clearColor) const;

    private:

      Renderquad(const Renderquad&);
      Renderquad& operator=(const Renderquad&);

      void createFramebuffer();

      util::SharedPointer<db::Texture2D> m_depthTexture;
      std::vector<util::SharedPointer<db::Texture2D>> m_readTextures;
      std::vector<util::SharedPointer<db::Texture2D>> m_writeTextures;

      GLuint m_color_tex;
      GLuint m_rboIndex;
      GLuint m_vboindex;
      GLuint m_fboIndex;
    };
  }
}

#endif
