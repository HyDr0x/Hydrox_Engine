#include "Renderer/Traverser/FrustumCullingTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

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

    bool FrustumCullingTraverser::preTraverse(const IRenderNode* treeNode)
    {
      treeNode->frustumCulling();

      return true;
    }
  }
}