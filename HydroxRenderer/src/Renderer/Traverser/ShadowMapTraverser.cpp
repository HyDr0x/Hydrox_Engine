#include "Renderer/Traverser/ShadowMapTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

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

    bool ShadowMapTraverser::preTraverse(IRenderNode* treeNode)
    {
      treeNode->rasterizeGeometry();

      return true;
    }
  }
}