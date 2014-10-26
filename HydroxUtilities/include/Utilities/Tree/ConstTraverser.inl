#include "Utilities/Tree/TreeNode.h"
#include "Utilities/Tree/GroupNode.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> ConstTraverser<BaseTraverser>::ConstTraverser(const TreeNodeAllocator<BaseTraverser>& allocator) : m_allocator(allocator), m_stopTraversal(false)
    {
    }

    template<class BaseTraverser> ConstTraverser<BaseTraverser>::~ConstTraverser()
    {
    }

    template<class BaseTraverser> void ConstTraverser<BaseTraverser>::doAscend(const TreeNode<BaseTraverser> &treeNode)
    {
      NodeIndex index = treeNode.getParent();
      while(index != ~0)
      {
        const TreeNode<BaseTraverser>& treeNode2 = m_allocator[index];

        if(!treeNode2.ascendTraverse(this) || m_stopTraversal)
        {
          return;
        }

        index = treeNode2.getParent();
      }

      postAscendTraverse();
    }

    template<class BaseTraverser> void ConstTraverser<BaseTraverser>::doTraverse(const TreeNode<BaseTraverser> &treeNode)
    {
      if(treeNode.preTraverse(this))
      {
        doTraverseDown(treeNode.getFirstChild());
      }
      treeNode.postTraverse(this);
    }

    template<class BaseTraverser> void ConstTraverser<BaseTraverser>::doTraverseDown(NodeIndex nodeIndex)
    {
      while(nodeIndex != ~0)
      {
        const TreeNode<BaseTraverser>& treeNode = m_allocator[nodeIndex];

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

    template<class BaseTraverser> void ConstTraverser<BaseTraverser>::postAscendTraverse()
    {
    }

    template<class BaseTraverser> bool ConstTraverser<BaseTraverser>::ascendTraverse(const TreeNode<BaseTraverser>& treeNode)
    {
      return true;
    }

    template<class BaseTraverser> bool ConstTraverser<BaseTraverser>::preTraverse(const TreeNode<BaseTraverser>& treeNode)
    {
      return true;
    }

    template<class BaseTraverser> void ConstTraverser<BaseTraverser>::postTraverse(const TreeNode<BaseTraverser>& treeNode)
    {
    }

    template<class BaseTraverser> bool ConstTraverser<BaseTraverser>::ascendTraverse(const GroupNode<BaseTraverser>& treeNode)
    {
      return true;
    }

    template<class BaseTraverser> bool ConstTraverser<BaseTraverser>::preTraverse(const GroupNode<BaseTraverser>& treeNode)
    {
      return true;
    }

    template<class BaseTraverser> void ConstTraverser<BaseTraverser>::postTraverse(const GroupNode<BaseTraverser>& treeNode)
    {
    }
  }
}