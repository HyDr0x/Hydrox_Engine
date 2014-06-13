#include "Renderer/Pipeline/Renderquad.h"

#include <Utilities/Math/Math.hpp>

#include <DataBase/RenderShader.h>
#include <DataBase/Texture2D.h>

namespace he
{
  namespace renderer
  {
    Renderquad::Renderquad() : m_fboIndex(~0), m_depthTexture(nullptr)
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

    void Renderquad::setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, std::vector<util::SharedPointer<db::Texture2D>> textures)
    {
      assert(textures.size() > 0 || m_depthTexture);

      m_depthTexture = depthTexture;

      m_writeTextures = textures;

      createFramebuffer();
    }

    void Renderquad::setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, int count, ...)
    {
      assert(count > 0 || m_depthTexture);

      m_depthTexture = depthTexture;

      va_list texList;
      va_start(texList, count);

      m_writeTextures.resize(count);
      for(unsigned int i = 0; i < m_writeTextures.size(); i++)
      {
        m_writeTextures[i] = va_arg(texList, util::SharedPointer<db::Texture2D>);
      }

      createFramebuffer();
    }

    void Renderquad::setRenderTargets(RenderOptions options, std::vector<util::SharedPointer<db::Texture2D>> textures)
    {
      assert(textures.size() > 0);

      m_depthTexture = util::SharedPointer<db::Texture2D>();
      m_writeTextures = textures;

      createFramebuffer();
    }

    void Renderquad::setRenderTargets(RenderOptions options, int count, ...)
    {
      assert(count > 0);

      va_list texList;
      va_start(texList, count);

      m_depthTexture = util::SharedPointer<db::Texture2D>();

      m_writeTextures.resize(count);
      for(unsigned int i = 0; i < m_writeTextures.size(); i++)
      {
        m_writeTextures[i] = va_arg(texList, util::SharedPointer<db::Texture2D>);
      }

      createFramebuffer();
    }

    void Renderquad::createFramebuffer()
    {
      if(m_fboIndex == ~0)
      {
        glGenFramebuffers(1, &m_fboIndex);
      }

      glBindFramebuffer(GL_FRAMEBUFFER, m_fboIndex);
      for(unsigned int i = 0; i < m_writeTextures.size(); i++)
      {
        m_writeTextures[i]->bindToFramebuffer(GL_COLOR_ATTACHMENT0 + i);
      }

      if(m_depthTexture)
      {
        m_depthTexture->bindToFramebuffer(GL_DEPTH_ATTACHMENT);
      }

      GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

      switch(fboStatus)
      {
      case GL_FRAMEBUFFER_COMPLETE:
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cerr << "Error, framebuffer incomplete attachment." << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cerr << "Error, framebuffer missing attachment." << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cerr << "Error, framebuffer incomplete draw buffer." << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cerr << "Error, framebuffer incomplete read buffer." << std::endl;
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cerr << "Error, framebuffer unsupported." << std::endl;
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        std::cerr << "Error, framebuffer incomplete multisample." << std::endl;
        break;
      case GL_FRAMEBUFFER_UNDEFINED:
      default:
        std::cerr << "Unknown error, framebuffer corrupted." << std::endl;
        break;
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderquad::setReadTextures(std::vector<util::SharedPointer<db::Texture2D>> textures)
    {
      m_readTextures = textures;
    }

    void Renderquad::setReadTextures(int count, ...)
    {
      va_list texList;
      va_start(texList, count);

      m_readTextures.resize(count);
      for(unsigned int i = 0; i < m_readTextures.size(); i++)
      {
        m_readTextures[i] = va_arg(texList, util::SharedPointer<db::Texture2D>);
      }
    }
  
    void Renderquad::setWriteFrameBuffer() const
    {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboIndex);

      if(!m_depthTexture)
      {
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
      }

      std::vector<GLenum> fboBuffer(m_writeTextures.size());
      for(unsigned int i = 0; i < m_writeTextures.size(); i++)
      {
        fboBuffer[i] = GL_COLOR_ATTACHMENT0 + i;
      }
      glDrawBuffers(fboBuffer.size(), &fboBuffer[0]);
    }

    void Renderquad::unsetWriteFrameBuffer() const
    {
      if(!m_depthTexture)
      {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
      }

      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

      GLenum windowBuff[] = { GL_BACK_LEFT };//switching to back buffer (LEFT/RIGHT --> for stereoscopic rendering, default is LEFT)
      glDrawBuffers(1, windowBuff);

      //glBindTexture(GL_TEXTURE_2D, m_depthTexture->m_texIndex);
      //std::vector<float> depth(1024 * 768);
      //glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &depth[0]);
      //glBindTexture(GL_TEXTURE_2D, 0);

      //float u;
      //for(unsigned int i = 0; i < depth.size(); i++)
      //{
      //  if(depth[i] != 1.0f)
      //  {
      //    u = depth[i];
      //  }
      //}
    }

    void Renderquad::render() const
    {    
      for(unsigned int i = 0; i < m_readTextures.size(); i++)
      {
        m_readTextures[i]->setTexture(i, i);
      }

      glEnableVertexAttribArray(db::RenderShader::SPECIAL0);
      glBindBuffer(GL_ARRAY_BUFFER, m_vboindex);

      glDrawArrays(GL_POINTS, 0, 1);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(db::RenderShader::SPECIAL0);

      for(unsigned int i = 0; i < m_readTextures.size(); i++)
      {
        m_readTextures[i]->unsetTexture();
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