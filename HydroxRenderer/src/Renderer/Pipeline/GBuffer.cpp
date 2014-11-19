#include "Renderer/Pipeline/GBuffer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/Texture2D.h>

#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/Traverser/RemoveGeometryTraverser.h"
#include "Renderer/Traverser/RenderGeometryTraverser.h"
#include "Renderer/Traverser/FrustumCullingTraverser.h"

#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

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

      RenderOptions *options = m_singletonManager->getService<RenderOptions>();

      m_depthTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32, nullptr));
      m_colorTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
      m_normalTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
      m_materialTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));
      
      m_fullscreenRenderQuad.setRenderTargets(m_depthTexture, 3, m_colorTexture, m_normalTexture, m_materialTexture);
    }

    void GBuffer::setGBuffer() const
    {
      m_fullscreenRenderQuad.setWriteFrameBuffer();
    }

    void GBuffer::unsetGBuffer() const
    {
      m_fullscreenRenderQuad.unsetWriteFrameBuffer();
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
      m_fullscreenRenderQuad.clearTargets(1.0f, std::vector<util::vec4f>(4, m_clearColor));
    }
  }
}