#ifndef RENDERQUAD_H_
#define RENDERQUAD_H_

#include <vector>
#include <iostream>
#include <cassert>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI Renderquad
    {
    public:

      Renderquad();
      ~Renderquad();

      virtual void setReadFrameBuffer(GLenum buffer) const;
      virtual void unsetReadFrameBuffer() const;

      virtual void setWriteFrameBuffer(std::vector<unsigned int> indices = std::vector<unsigned int>()) const;
      virtual void unsetWriteFrameBuffer() const;

      void setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, int count, ...);
      void setRenderTargets(util::SharedPointer<db::Texture3D> depthTexture3D, unsigned int layer, int count, ...);
      void setRenderTargets(int count, ...);

      void setRenderTargets3D(util::SharedPointer<db::Texture2D> depthTexture, int count, ...);
      void setRenderTargets3D(util::SharedPointer<db::Texture3D> depthTexture3D, unsigned int layer, int count, ...);
      void setRenderTargets3D(int count, ...);

      void setDepthLayer(unsigned int layer);

      virtual void render() const;

      void clearTargets(GLfloat clearDepthValue, std::vector<util::vec4f>& clearColors, bool clearDepth) const;
      void clearTargets(GLfloat clearDepthValue, std::vector<util::vec4ui>& clearColors, bool clearDepth) const;
      void clearTargets(GLfloat clearDepthValue, util::vec4f clearColor, std::vector<unsigned int> indices = std::vector<unsigned int>(), bool clearDepth = true) const;

    private:

      Renderquad(const Renderquad&);
      Renderquad& operator=(const Renderquad&);

      virtual void createFramebuffer(unsigned int layer);

      util::SharedPointer<db::Texture2D> m_depthTexture;
      util::SharedPointer<db::Texture3D> m_depthTexture3D;

      std::vector<util::SharedPointer<db::Texture2D>> m_writeTextures;
      std::vector<util::SharedPointer<db::Texture3D>> m_writeTextures3D;

      GLuint m_vboindex;
      GLuint m_fboIndex;
    };
  }
}

#endif
