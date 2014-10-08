#include "Renderer/Traverser/UpdateTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    UpdateTraverser::UpdateTraverser()
    {
    }

    UpdateTraverser::~UpdateTraverser()
    {
    }

    bool UpdateTraverser::preTraverse(RenderNode* treeNode)
    {
      treeNode->getRenderGroup()->updateBuffer();

      return true;
    }
  }
}