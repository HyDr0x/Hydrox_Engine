#include "Renderer/Traverser/UpdateTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

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

    bool UpdateTraverser::preTraverse(IRenderNode* treeNode)
    {
      treeNode->updateBuffer();

      return true;
    }
  }
}