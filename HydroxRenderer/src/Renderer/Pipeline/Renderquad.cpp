#include "Renderer/Pipeline/Renderquad.h"

#include <Utilities/Math/Math.hpp>

#include <DataBase/RenderShader.h>
#include <DataBase/Texture2D.h>

namespace he
{
  namespace renderer
  {
    Renderquad::Renderquad() : m_fboIndex(~0)
    {
      glGenBuffers(1, &m_vboindex);
      glBindBuffer(GL_ARRAY_BUFFER, m_vboindex);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
      glVertexAttribPointer(db::RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
      glBindBuffer(GL_ARRAY_BUFFER, 0);    
    }

    Renderquad::~Renderquad()
    {
      glDeleteBuffers(1, &m_vboindex);

      if(m_fboIndex != ~0)
      {
        glDeleteFramebuffers(1, &m_fboIndex);
      }
    }

    void Renderquad::clearTargets(GLfloat clearDepthValue, std::vector<util::Vector<float, 4>>& clearColors) const
    {
      setWriteFrameBuffer();

      glClearBufferfv(GL_DEPTH, 0, &clearDepthValue);

      for(unsigned int i = 0; i < clearColors.size(); i++)
      {
        glClearBufferfv(GL_COLOR, i, &clearColors[i][0]);
      }

      unsetWriteFrameBuffer();
    }

    void Renderquad::clearTargets(GLfloat clearDepthValue, util::Vector<float, 4> clearColor) const
    {
      setWriteFrameBuffer();

      glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      unsetWriteFrameBuffer();
    }
  }
}