#ifndef UCONSTTRAVERSER_H_
#define UCONSTTRAVERSER_H_

#include "Utilities/Tree/NodeIndex.h"

#include "Utilities/Tree/TreeNodeAllocator.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> class TreeNode;
    template<class BaseTraverser> class GroupNode;

    template<class BaseTraverser> class ConstTraverser : public BaseTraverser
    {
    public:

      ConstTraverser(const TreeNodeAllocator<BaseTraverser>& allocator);
      virtual ~ConstTraverser() = 0;

      virtual void doAscend(const TreeNode<BaseTraverser>& treeNode);//starts with the parent node of the given treeNode

      virtual void doTraverse(const TreeNode<BaseTraverser>& treeNode);//starts with the given tree node, but without its siblings

      virtual bool ascendTraverse(const TreeNode<BaseTraverser>& treeNode);
      virtual bool preTraverse(const TreeNode<BaseTraverser>& treeNode);
      virtual void postTraverse(const TreeNode<BaseTraverser>& treeNode);

      virtual bool ascendTraverse(const GroupNode<BaseTraverser>& treeNode);
      virtual bool preTraverse(const GroupNode<BaseTraverser>& treeNode);
      virtual void postTraverse(const GroupNode<BaseTraverser>& treeNode);

    protected:

      void doTraverseDown(NodeIndex nodeIndex);

      virtual void postAscendTraverse();

      const TreeNodeAllocator<BaseTraverser>& m_allocator;

      bool m_stopTraversal;
    };
  }
}

#include "Utilities/Tree/ConstTraverser.inl"

#endif
