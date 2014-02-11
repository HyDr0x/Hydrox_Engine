#include "Renderer/RenderTree/Traverser.h"

#include "Renderer/RenderTree/TreeNode.h"
#include "Renderer/RenderTree/GroupNode.h"

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

    void Traverser::doTraverse(TreeNode* treeNode)
    {
      if(treeNode != nullptr)
      {
        if(treeNode->preTraverse(this))
        {
          doTraverseDown(treeNode->getFirstChild());
        }
        treeNode->postTraverse(this);
      }
    }

    void Traverser::doTraverseDown(TreeNode* treeNode)
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

    bool Traverser::preTraverse(RenderNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(RenderNode* treeNode)
    {
    }

    bool Traverser::preTraverse(StaticRenderNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(StaticRenderNode* treeNode)
    {
    }

    bool Traverser::preTraverse(AnimatedRenderNode* treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(AnimatedRenderNode* treeNode)
    {
    }
	}
}