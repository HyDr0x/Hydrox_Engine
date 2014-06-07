#include "Renderer/Pipeline/GBuffer.h"

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
#include "Renderer/Resources/Texture2D.h"

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

    void GBuffer::initialize(const RenderOptions& renderingOptions, util::SingletonManager *singletonManager, util::ResourceHandle gBufferShaderHandle)
    {
      m_depthTexture = util::SharedPointer<Texture2D>(new Texture2D(renderingOptions.width, renderingOptions.height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32, nullptr, false));
      m_colorTexture = util::SharedPointer<Texture2D>(new Texture2D(renderingOptions.width, renderingOptions.height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA8, GL_RGBA, 4, 32, nullptr, false));
      m_normalTexture = util::SharedPointer<Texture2D>(new Texture2D(renderingOptions.width, renderingOptions.height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA8, GL_RGBA, 4, 32, nullptr, false));
      
      m_gBufferShaderHandle = gBufferShaderHandle;

      m_fullscreenRenderQuad.initialize(singletonManager);
      m_fullscreenRenderQuad.setRenderTargets(m_depthTexture, 2, m_colorTexture, m_normalTexture);
    }

    void GBuffer::setGBuffer() const
    {
      m_fullscreenRenderQuad.setWriteFrameBuffer();
    }

    void GBuffer::unsetGBuffer() const
    {
      m_fullscreenRenderQuad.unsetWriteFrameBuffer();
    }

    void GBuffer::render() const
    {     
      m_fullscreenRenderQuad.renderReadFromTextures(m_gBufferShaderHandle);
    }

    void GBuffer::clear() const
    {
      std::vector<util::Vector<float, 4>> clearColors(2);
      clearColors[0] = util::Vector<float, 4>(0, 0, 0, 0);
      clearColors[1] = util::Vector<float, 4>(0, 0, 0, 0);
      m_fullscreenRenderQuad.clearTargets(1.0f, clearColors);
    }
  }
}