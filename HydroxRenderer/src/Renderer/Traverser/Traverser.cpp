#include "Renderer/Traverser/Traverser.h"

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
	namespace renderer
	{
    Traverser::Traverser() : m_stopTraversal(false)
    {
    }

    Traverser::~Traverser()
    {
    }

    void Traverser::doTraverse(TreeNode *treeNode)
    {
      if(treeNode->preTraverse(this))
      {
        doTraverseDown(treeNode->getFirstChild());
      }
      treeNode->postTraverse(this);
    }

    void Traverser::doTraverseDown(TreeNode* treeNode)
    {
      while(treeNode != nullptr)
      {
        if(treeNode->preTraverse(this))
        {
          doTraverseDown(treeNode->getFirstChild());
        }

        treeNode->postTraverse(this);
        treeNode = treeNode->getNextSibling();

        if(m_stopTraversal)
        {
          return;
        }
      }
    }

    bool Traverser::preTraverse(TreeNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(TreeNode* treeNode)
    {
    }

    bool Traverser::preTraverse(GroupNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(GroupNode* treeNode)
    {
    }

    bool Traverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(VertexDeclarationNode* treeNode)
    {
    }

    bool Traverser::preTraverse(ShaderNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(ShaderNode* treeNode)
    {
    }

    bool Traverser::preTraverse(TextureNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(TextureNode* treeNode)
    {
    }

    bool Traverser::preTraverse(IRenderNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(IRenderNode* treeNode)
    {
    }
	}
}