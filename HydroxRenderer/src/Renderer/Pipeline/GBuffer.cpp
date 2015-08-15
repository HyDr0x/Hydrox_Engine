#include "Renderer/Pipeline/GBuffer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Texture2D.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    GBuffer::GBuffer()
    {
    }

    GBuffer::~GBuffer()
    {
    }

    void GBuffer::initialize(util::SingletonManager *singletonManager)
    {
      m_singletonManager = singletonManager;

      util::SharedPointer<RenderOptions> options = m_singletonManager->getService<RenderOptions>();

      m_depthTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
      m_colorTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_normalTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_materialTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      
      m_fullscreenRenderQuad.setRenderTargets(m_depthTexture, 3, m_colorTexture, m_normalTexture, m_materialTexture);
      m_debugRenderquad.setRenderTargets(m_depthTexture, 1, m_colorTexture);
    }

    void GBuffer::setGBuffer() const
    {
      m_fullscreenRenderQuad.setWriteFrameBuffer();
    }

    void GBuffer::unsetGBuffer() const
    {
      m_fullscreenRenderQuad.unsetWriteFrameBuffer();
    }

    void GBuffer::setDebugGBuffer() const
    {
      m_debugRenderquad.setWriteFrameBuffer();
    }

    void GBuffer::unsetDebugGBuffer() const
    {
      m_debugRenderquad.unsetWriteFrameBuffer();
    }

    void GBuffer::setClearColor(util::vec4f clearColor)
    {
      m_clearColor = clearColor;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getDepthTexture() const
    {
      return m_depthTexture;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getColorTexture() const
    {
      return m_colorTexture;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getNormalTexture() const
    {
      return m_normalTexture;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getMaterialTexture() const
    {
      return m_materialTexture;
    }

    void GBuffer::clear() const
    {
      m_fullscreenRenderQuad.clearTargets(1.0f, std::vector<util::vec4f>(3, m_clearColor), true);
    }
  }
}