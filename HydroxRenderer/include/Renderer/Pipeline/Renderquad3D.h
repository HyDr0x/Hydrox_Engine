#ifndef RENDERQUAD3D_H_
#define RENDERQUAD3D_H_

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
    class GRAPHICAPI Renderquad3D : public Renderquad
    {
    public:

      Renderquad3D();
      ~Renderquad3D();

      virtual void setWriteFrameBuffer(std::vector<unsigned int> indices = std::vector<unsigned int>()) const override;
      virtual void unsetWriteFrameBuffer() const override;

      void setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, util::SharedPointer<db::Texture3D> texture);
      void setRenderTargets(util::SharedPointer<db::Texture3D> texture);

      void setReadTextures(util::SharedPointer<db::Texture3D> texture);

      virtual void render() const override;

    private:

      Renderquad3D(const Renderquad3D&);
      Renderquad3D& operator=(const Renderquad3D&);

      virtual void createFramebuffer() override;

      util::SharedPointer<db::Texture2D> m_depthTexture;
      util::SharedPointer<db::Texture3D> m_readTexture;
      util::SharedPointer<db::Texture3D> m_writeTexture;
    };
  }
}

#endif
