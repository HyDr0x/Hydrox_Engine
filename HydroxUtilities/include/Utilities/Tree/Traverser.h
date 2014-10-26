#ifndef UTRAVERSER_H_
#define UTRAVERSER_H_

#include "Utilities/Tree/NodeIndex.h"

#include "Utilities/Tree/TreeNodeAllocator.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> class TreeNode;
    template<class BaseTraverser> class GroupNode;

    template<class BaseTraverser> class Traverser : public BaseTraverser
    {
    public:

      Traverser(TreeNodeAllocator<BaseTraverser>& allocator);
      virtual ~Traverser() = 0;

      virtual void doAscend(TreeNode<BaseTraverser>& treeNode);//starts with the parent node of the given treeNode

      virtual void doTraverse(TreeNode<BaseTraverser>& treeNode);//starts with the given tree node, but without its siblings

      virtual bool ascendTraverse(TreeNode<BaseTraverser>& treeNode);
      virtual bool preTraverse(TreeNode<BaseTraverser>& treeNode);
      virtual void postTraverse(TreeNode<BaseTraverser>& treeNode);

      virtual bool ascendTraverse(GroupNode<BaseTraverser>& treeNode);
      virtual bool preTraverse(GroupNode<BaseTraverser>& treeNode);
      virtual void postTraverse(GroupNode<BaseTraverser>& treeNode);

    protected:

      void doTraverseDown(NodeIndex nodeIndex);//starts with the given treeNode and traverses its siblings too

      virtual void postAscendTraverse();

      TreeNodeAllocator<BaseTraverser>& m_allocator;

      bool m_stopTraversal;
    };
  }
}

#include "Utilities/Tree/Traverser.inl"

#endif
