#include "Renderer/Traverser/ShadowMapTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    ShadowMapTraverser::ShadowMapTraverser()
    {
    }

    ShadowMapTraverser::~ShadowMapTraverser()
    {
    }

    bool ShadowMapTraverser::preTraverse(RenderNode* treeNode)
    {
      treeNode->getRenderGroup()->rasterizeGeometry();

      return true;
    }
  }
}