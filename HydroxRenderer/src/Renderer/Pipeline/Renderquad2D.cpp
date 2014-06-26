#include "Renderer/Pipeline/Renderquad2D.h"

#include <Utilities/Math/Math.hpp>

#include <DataBase/RenderShader.h>
#include <DataBase/Texture2D.h>

namespace he
{
  namespace renderer
  {
    Renderquad2D::Renderquad2D() : m_depthTexture(nullptr)
    {
    }

    Renderquad2D::~Renderquad2D()
    {
    }

    void Renderquad2D::setWriteFrameBuffer(std::vector<unsigned int> indices) const
    {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboIndex);

      if(!m_depthTexture)
      {
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
      }

      if(indices.empty())
      {
        indices.resize(m_writeTextures.size());
        for(unsigned int i = 0; i < indices.size(); i++)
        {
          indices[i] = i;
        }
      }

      std::vector<GLenum> fboBuffer(m_writeTextures.size());
      for(unsigned int i = 0; i < m_writeTextures.size(); i++)
      {
        fboBuffer[indices[i]] = GL_COLOR_ATTACHMENT0 + indices[i];
      }
      glDrawBuffers(fboBuffer.size(), &fboBuffer[0]);
    }

    void Renderquad2D::unsetWriteFrameBuffer() const
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

    void Renderquad2D::setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, std::vector<util::SharedPointer<db::Texture2D>> textures)
    {
      assert(textures.size() > 0 || m_depthTexture);

      m_depthTexture = depthTexture;

      m_writeTextures = textures;

      createFramebuffer();
    }

    void Renderquad2D::setRenderTargets(util::SharedPointer<db::Texture2D> depthTexture, int count, ...)
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

    void Renderquad2D::setRenderTargets(std::vector<util::SharedPointer<db::Texture2D>> textures)
    {
      assert(textures.size() > 0);

      m_depthTexture = util::SharedPointer<db::Texture2D>();
      m_writeTextures = textures;

      createFramebuffer();
    }

    void Renderquad2D::setRenderTargets(int count, ...)
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

    void Renderquad2D::createFramebuffer()
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

    void Renderquad2D::setReadTextures(std::vector<util::SharedPointer<db::Texture2D>> textures)
    {
      m_readTextures = textures;
    }

    void Renderquad2D::setReadTextures(int count, ...)
    {
      va_list texList;
      va_start(texList, count);

      m_readTextures.resize(count);
      for(unsigned int i = 0; i < m_readTextures.size(); i++)
      {
        m_readTextures[i] = va_arg(texList, util::SharedPointer<db::Texture2D>);
      }
    }
  
    void Renderquad2D::render() const
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
  }
}