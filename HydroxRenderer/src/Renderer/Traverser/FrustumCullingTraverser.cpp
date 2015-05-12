#include "Renderer/Traverser/FrustumCullingTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    FrustumCullingTraverser::FrustumCullingTraverser()
    {
    }

    FrustumCullingTraverser::~FrustumCullingTraverser()
    {
    }

    bool FrustumCullingTraverser::preTraverse(const RenderNode *treeNode)
    {
      treeNode->getRenderGroup()->frustumCulling();

      return true;
    }
  }
}