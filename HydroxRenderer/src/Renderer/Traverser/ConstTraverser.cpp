#include "Renderer/Traverser/ConstTraverser.h"

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"

namespace he
{
  namespace renderer
  {

    ConstTraverser::ConstTraverser() : m_stopTraversal(false)
    {
    }

    ConstTraverser::~ConstTraverser()
    {
    }

    void ConstTraverser::doTraverse(util::SharedPointer<const TreeNode> treeNode)
    {
      if(treeNode->preTraverse(this))
      {
        doTraverseDown(treeNode->getFirstChild().dynamic_pointer_cast<const TreeNode>());
      }
      treeNode->postTraverse(this);
    }

    void ConstTraverser::doTraverseDown(util::SharedPointer<const TreeNode> treeNode)
    {
      while(treeNode)
      {
        if(treeNode->preTraverse(this))
        {
          doTraverseDown(treeNode->getFirstChild().dynamic_pointer_cast<const TreeNode>());
        }

        util::SharedPointer<TreeNode> node = treeNode->getNextSibling();
        treeNode->postTraverse(this);
        treeNode = node.dynamic_pointer_cast<const TreeNode>();

        if(m_stopTraversal)
        {
          return;
        }
      }
    }

    bool ConstTraverser::preTraverse(const TreeNode *treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const TreeNode *treeNode)
    {
    }

    bool ConstTraverser::preTraverse(const GroupNode * treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const GroupNode * treeNode)
    {
    }

    bool ConstTraverser::preTraverse(const VertexDeclarationNode * treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const VertexDeclarationNode * treeNode)
    {
    }


    bool ConstTraverser::preTraverse(const ShaderNode * treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const ShaderNode * treeNode)
    {
    }

    bool ConstTraverser::preTraverse(const TextureNode * treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const TextureNode * treeNode)
    {
    }

    bool ConstTraverser::preTraverse(const RenderNode * treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const RenderNode * treeNode)
    {
    }
  }
}