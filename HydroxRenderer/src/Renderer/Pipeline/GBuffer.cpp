#include "Renderer/Pipeline/GBuffer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/Traverser/InsertStaticGeometryTraverser.h"
#include "Renderer/Traverser/InsertSkinnedGeometryTraverser.h"
#include "Renderer/Traverser/RemoveGeometryTraverser.h"
#include "Renderer/Traverser/RenderGeometryTraverser.h"
#include "Renderer/Traverser/FrustumCullingTraverser.h"
#include "Renderer/Traverser/DeleteTraverser.h"

#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"
#include <DataBase/Texture2D.h>

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

    void GBuffer::initialize(const RenderOptions& renderingOptions, util::SingletonManager *singletonManager)
    {
      m_singletonManager = singletonManager;

      m_depthTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(renderingOptions.width, renderingOptions.height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32, nullptr, false));
      m_colorTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(renderingOptions.width, renderingOptions.height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA8, GL_RGBA, 4, 32, nullptr, false));
      m_normalTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(renderingOptions.width, renderingOptions.height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA8, GL_RGBA, 4, 32, nullptr, false));
      m_materialTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(renderingOptions.width, renderingOptions.height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA8, GL_RGBA, 4, 32, nullptr, false));

      m_fullscreenRenderQuad.setRenderTargets(m_depthTexture, 3, m_colorTexture, m_normalTexture, m_materialTexture);
      m_fullscreenRenderQuad.setReadTextures(4, m_depthTexture, m_colorTexture, m_normalTexture, m_materialTexture);
    }

    void GBuffer::setGBuffer() const
    {
      m_fullscreenRenderQuad.setWriteFrameBuffer();
    }

    void GBuffer::unsetGBuffer() const
    {
      m_fullscreenRenderQuad.unsetWriteFrameBuffer();
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
      m_fullscreenRenderQuad.clearTargets(1.0f, std::vector<util::Vector<float, 4>>(3, util::Vector<float, 4>(0, 0, 0, 0)));
    }
  }
}