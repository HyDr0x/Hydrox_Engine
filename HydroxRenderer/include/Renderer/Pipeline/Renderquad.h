#ifndef RENDERQUAD_H_
#define RENDERQUAD_H_

#include <vector>
#include <iostream>
#include <stdarg.h>
#include <assert.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

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

      virtual void setWriteFrameBuffer(std::vector<unsigned int> indices = std::vector<unsigned int>()) const;
      virtual void unsetWriteFrameBuffer() const;

      void setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, int count, ...);
      void setRenderTargets(util::SharedPointer<db::Texture3D> depthTexture3D, unsigned int layer, int count, ...);
      void setRenderTargets(int count, ...);
      void setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, util::SharedPointer<db::Texture3D> writeTexture3D);
      void setRenderTargets(util::SharedPointer<db::Texture3D> depthTexture3D, unsigned int layer, util::SharedPointer<db::Texture3D> writeTexture3D);
      void setRenderTargets(util::SharedPointer<db::Texture3D> writeTexture3D);

      virtual void render() const;

      void clearTargets(GLfloat clearDepthValue, std::vector<util::Vector<float, 4>>& clearColors) const;
      void clearTargets(GLfloat clearDepthValue, util::Vector<float, 4> clearColor, std::vector<unsigned int> indices = std::vector<unsigned int>()) const;

    private:

      Renderquad(const Renderquad&);
      Renderquad& operator=(const Renderquad&);

      virtual void createFramebuffer(unsigned int layer);

      util::SharedPointer<db::Texture2D> m_depthTexture;
      util::SharedPointer<db::Texture3D> m_depthTexture3D;

      std::vector<util::SharedPointer<db::Texture2D>> m_writeTextures;
      util::SharedPointer<db::Texture3D> m_writeTexture3D;

      GLuint m_vboindex;
      GLuint m_fboIndex;
    };
  }
}

#endif
