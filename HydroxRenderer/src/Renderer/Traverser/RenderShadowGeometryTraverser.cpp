#include "Renderer/Traverser/RenderShadowGeometryTraverser.h"

#include <Shader/ShaderContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    RenderShadowGeometryTraverser::RenderShadowGeometryTraverser()
    {
    }

    RenderShadowGeometryTraverser::~RenderShadowGeometryTraverser()
    {
    }

    void RenderShadowGeometryTraverser::initialize(util::SingletonManager *singletonManager)
    {
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_options = singletonManager->getService<RenderOptions>();
    }

    void RenderShadowGeometryTraverser::setViewProjectionIndex(int viewProjectionIndex)
    {
      m_viewProjectionIndex = viewProjectionIndex;
    }

    bool RenderShadowGeometryTraverser::preTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->setVertexArray();

      return true;
    }

    void RenderShadowGeometryTraverser::postTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->unsetVertexArray();
    }

    bool RenderShadowGeometryTraverser::preTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useShader();

      return true;
    }

    void RenderShadowGeometryTraverser::postTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useNoShader();
    }

    bool RenderShadowGeometryTraverser::preTraverse(RenderNode * treeNode)
    {
      treeNode->getRenderGroup()->rasterizeShadowGeometry();

      return true;
    }

    void RenderShadowGeometryTraverser::postTraverse(RenderNode * treeNode)
    {
    }
  }
}