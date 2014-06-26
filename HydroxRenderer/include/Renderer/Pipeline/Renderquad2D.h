#ifndef RENDERQUAD2D_H_
#define RENDERQUAD2D_H_

#include <vector>
#include <iostream>
#include <stdarg.h>
#include <assert.h>

#include "Renderer/Pipeline/Renderquad.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI Renderquad2D : public Renderquad
    {
    public:

      Renderquad2D();
      ~Renderquad2D();

      virtual void setWriteFrameBuffer(std::vector<unsigned int> indices = std::vector<unsigned int>()) const override;
      virtual void unsetWriteFrameBuffer() const override;

      void setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, std::vector<util::SharedPointer<db::Texture2D>> textures);
      void setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, int count, ...);

      void setRenderTargets(std::vector<util::SharedPointer<db::Texture2D>> textures);
      void setRenderTargets(int count, ...);

      void setReadTextures(std::vector<util::SharedPointer<db::Texture2D>> textures);
      void setReadTextures(int count, ...);

      virtual void render() const override;

    private:

      Renderquad2D(const Renderquad2D&);
      Renderquad2D& operator=(const Renderquad2D&);

      virtual void createFramebuffer() override;

      util::SharedPointer<db::Texture2D> m_depthTexture;
      std::vector<util::SharedPointer<db::Texture2D>> m_readTextures;
      std::vector<util::SharedPointer<db::Texture2D>> m_writeTextures;
    };
  }
}

#endif
