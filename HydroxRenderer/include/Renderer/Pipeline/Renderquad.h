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
      virtual ~Renderquad();

      virtual void setWriteFrameBuffer(std::vector<unsigned int> indices = std::vector<unsigned int>()) const = 0;
      virtual void unsetWriteFrameBuffer() const = 0;

      virtual void render() const = 0;

      void clearTargets(GLfloat clearDepthValue, std::vector<util::Vector<float, 4>>& clearColors) const;
      void clearTargets(GLfloat clearDepthValue, util::Vector<float, 4> clearColor) const;

    protected:

      Renderquad(const Renderquad&);
      Renderquad& operator=(const Renderquad&);

      virtual void createFramebuffer() = 0;

      GLuint m_color_tex;
      GLuint m_rboIndex;
      GLuint m_vboindex;
      GLuint m_fboIndex;
    };
  }
}

#endif
