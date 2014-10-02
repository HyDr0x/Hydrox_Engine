#include "Renderer/Traverser/DeleteTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
  namespace renderer
  {
    DeleteTraverser::DeleteTraverser()
    {
    }

    DeleteTraverser::~DeleteTraverser()
    {
    }

    void DeleteTraverser::doTraverseDown(TreeNode* treeNode)
    {
      while(treeNode != nullptr)
      {
        if(treeNode->preTraverse(this))
        {
          doTraverseDown(treeNode->getFirstChild());
        }

        TreeNode *node = treeNode->getNextSibling();
        treeNode->postTraverse(this);
        treeNode = node;

        if(m_stopTraversal)
        {
          return;
        }
      }
    }

    void DeleteTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    void DeleteTraverser::postTraverse(ShaderNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    void DeleteTraverser::postTraverse(TextureNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    void DeleteTraverser::postTraverse(IRenderNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }
  }
}