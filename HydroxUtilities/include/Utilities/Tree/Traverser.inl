#include "Utilities/Tree/TreeNode.h"
#include "Utilities/Tree/GroupNode.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> Traverser<BaseTraverser>::Traverser(TreeNodeAllocator<BaseTraverser>& allocator) : m_allocator(allocator), m_stopTraversal(false)
    {
    }

    template<class BaseTraverser> Traverser<BaseTraverser>::~Traverser()
    {
    }

    template<class BaseTraverser> void Traverser<BaseTraverser>::doAscend(TreeNode<BaseTraverser>& treeNode)
    {
      NodeIndex index = treeNode.getParent();
      while(index != ~0)//start calculating the trfMatrix of the upper path of the actual node
      {
        TreeNode<BaseTraverser>& treeNode2 = m_allocator[index];

        if(!treeNode2.ascendTraverse(this) || m_stopTraversal)
        {
          return;
        }

        index = treeNode2.getParent();
      }

      postAscendTraverse();
    }

    template<class BaseTraverser> void Traverser<BaseTraverser>::doTraverse(TreeNode<BaseTraverser>& treeNode)
    {
      if(treeNode.preTraverse(this))
      {
        doTraverseDown(treeNode.getFirstChild());
      }
      treeNode.postTraverse(this);
    }

    template<class BaseTraverser> void Traverser<BaseTraverser>::doTraverseDown(NodeIndex nodeIndex)
    {
      while(nodeIndex != ~0)
      {
        TreeNode<BaseTraverser>& treeNode = m_allocator[nodeIndex];

        if(treeNode.preTraverse(this))
        {
          doTraverseDown(treeNode.getFirstChild());
        }

        nodeIndex = treeNode.getNextSibling();
        treeNode.postTraverse(this);

        if(m_stopTraversal)
        {
          return;
        }
      }
    }

    template<class BaseTraverser> void Traverser<BaseTraverser>::postAscendTraverse()
    {
    }

    template<class BaseTraverser> bool Traverser<BaseTraverser>::ascendTraverse(TreeNode<BaseTraverser>& treeNode)
    {
      return true;
    }

    template<class BaseTraverser> bool Traverser<BaseTraverser>::preTraverse(TreeNode<BaseTraverser>& treeNode)
    {
      return true;
    }

    template<class BaseTraverser> void Traverser<BaseTraverser>::postTraverse(TreeNode<BaseTraverser>& treeNode)
    {
    }

    template<class BaseTraverser> bool Traverser<BaseTraverser>::ascendTraverse(GroupNode<BaseTraverser>& treeNode)
    {
      return true;
    }

    template<class BaseTraverser> bool Traverser<BaseTraverser>::preTraverse(GroupNode<BaseTraverser>& treeNode)
    {
      return true;
    }

    template<class BaseTraverser> void Traverser<BaseTraverser>::postTraverse(GroupNode<BaseTraverser>& treeNode)
    {
    }
  }
}