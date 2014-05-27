#include "Renderer/Pipeline/Renderquad.h"

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Resources/ResourceManager.hpp"
#include "Renderer/Resources/RenderShader.h"

namespace he
{
  namespace renderer
  {
    Renderquad::Renderquad() : m_fboIndex(~0), m_depthTexture(nullptr)
    {
	    glGenBuffers(1, &m_vboindex);
      glBindBuffer(GL_ARRAY_BUFFER, m_vboindex);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
      glVertexAttribPointer(RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
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

    void Renderquad::initialize(util::SingletonManager *singletonManager)
    {
      m_singletonManager = singletonManager;
    }

    void Renderquad::setRenderTargets(util::SharedPointer<Texture2D> depthTexture, std::vector<util::SharedPointer<Texture2D>> textures)
    {
	    assert(textures.size() > 0 || !m_depthTexture.isNullptr());

      m_depthTexture = depthTexture;

      m_textures = textures;

	    if(m_fboIndex == ~0)
      {
        glGenFramebuffers(1, &m_fboIndex);
      }

	    glBindFramebuffer(GL_FRAMEBUFFER, m_fboIndex);
		    for(unsigned int i = 0; i < m_textures.size(); i++)
        {
			    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->m_texIndex, 0);
        }
		    if(!m_depthTexture.isNullptr()) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->m_texIndex, 0);

        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
        {
          std::cerr << "Error, framebuffer corrupted." << std::endl;
        }
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderquad::setRenderTargets(util::SharedPointer<Texture2D> depthTexture, int count, ...)
    {
      assert(count > 0 || !m_depthTexture.isNullptr());

      m_depthTexture = depthTexture;

	    m_textures.resize(count);

	    if(m_fboIndex == ~0)
      {
        glGenFramebuffers(1, &m_fboIndex);
      }

      va_list texList;
	    va_start(texList, count);

	    glBindFramebuffer(GL_FRAMEBUFFER, m_fboIndex);
		    for(unsigned int i = 0; i < m_textures.size(); i++)
		    {
          m_textures[i] = va_arg(texList, util::SharedPointer<Texture2D>);
			    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->m_texIndex, 0);
		    }

		    if(!m_depthTexture.isNullptr()) 
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->m_texIndex, 0);

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
  
    void Renderquad::setWriteFrameBuffer() const
    {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboIndex);

      std::vector<GLenum> fboBuffer(m_textures.size());
		  for(unsigned int i = 0; i < m_textures.size(); i++)
      {
			  fboBuffer[i] = GL_COLOR_ATTACHMENT0 + i;
      }
		  glDrawBuffers(fboBuffer.size(), &fboBuffer[0]);
    }

    void Renderquad::unsetWriteFrameBuffer() const
    {
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

    void Renderquad::render(util::ResourceHandle shaderHandle) const
    {
      RenderShader *shader = m_singletonManager->getService<RenderShaderManager>()->getObject(shaderHandle);
      glDepthFunc(GL_ALWAYS);
      shader->useShader();

	    glEnableVertexAttribArray(RenderShader::SPECIAL0);
      glBindBuffer(GL_ARRAY_BUFFER, m_vboindex);

	    glDrawArrays(GL_POINTS, 0, 1);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(RenderShader::SPECIAL0);

      shader->useNoShader();
      glDepthFunc(GL_LESS);
    }

    void Renderquad::renderReadFromTextures(util::ResourceHandle shaderHandle) const
    {
      RenderShader *shader = m_singletonManager->getService<RenderShaderManager>()->getObject(shaderHandle);

      shader->useShader();

      unsigned int depthTextureOffset = 0;
      if(!m_depthTexture.isNullptr())
      {
        m_depthTexture->setTexture(0, 0);
        depthTextureOffset = 1;
      }
      
      for(unsigned int i = 0; i < m_textures.size(); i++)
      {
        m_textures[i]->setTexture(i + depthTextureOffset, i + depthTextureOffset);
      }

	    glEnableVertexAttribArray(RenderShader::SPECIAL0);
      glBindBuffer(GL_ARRAY_BUFFER, m_vboindex);

	    glDrawArrays(GL_POINTS, 0, 1);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(RenderShader::SPECIAL0);

      for(unsigned int i = 0; i < m_textures.size(); i++)
      {
        m_textures[i]->unsetTexture();
      }

      if(!m_depthTexture.isNullptr())
      {
        m_depthTexture->unsetTexture();
      }

      shader->useNoShader();
    }

    void Renderquad::clearTargets(float clearDepthValue, std::vector<util::Vector<float, 4>>& clearColors) const
    {
      setWriteFrameBuffer();

      glClearBufferfv(GL_DEPTH, 0, &clearDepthValue);

      for(unsigned int i = 0; i < clearColors.size(); i++)
		  {
        glClearBufferfv(GL_COLOR, i, &clearColors[i][0]);
		  }

      unsetWriteFrameBuffer();
    }
  }
}