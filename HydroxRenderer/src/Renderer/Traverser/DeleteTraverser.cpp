#include "Renderer/Traverser/DeleteTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

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

    bool DeleteTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(ShaderNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(ShaderNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(TextureNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(TextureNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(RenderNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(RenderNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }
	}
}