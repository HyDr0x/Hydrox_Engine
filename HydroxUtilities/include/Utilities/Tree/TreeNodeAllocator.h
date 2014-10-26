#ifndef UTREENODEALLOCATOR_H_
#define UTREENODEALLOCATOR_H_

#include <vector>
#include <list>
#include <assert.h>
#include <algorithm>
#include <new>

#include "Utilities/Math/Math.hpp"

#include "Utilities/Tree/TreeNodeFactory.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> class TreeNode;
    template<class BaseTraverser> class GroupNode;

    template<class BaseTraverser> class TreeNodeAllocator
    {
    public:

      TreeNodeAllocator(unsigned int nodeBlockSize = 256);
      TreeNodeAllocator(const TreeNodeAllocator<BaseTraverser>& other);
      virtual ~TreeNodeAllocator();

      TreeNodeAllocator<BaseTraverser>& operator=(TreeNodeAllocator<BaseTraverser> other);

      TreeNode<BaseTraverser>& insert(NodeIndex index);
      NodeIndex insert(const TreeNode<BaseTraverser>& treeNode);
      TreeNode<BaseTraverser>& insert(const TreeNode<BaseTraverser>& treeNode, NodeIndex index);
      void erase();
      void erase(NodeIndex index);

      TreeNode<BaseTraverser>& operator[](NodeIndex index);
      const TreeNode<BaseTraverser>& operator[](NodeIndex index) const;

      bool contains(NodeIndex index) const;

      void reorderTreeNodes();

      unsigned int getNodeBlockSize() const;
      unsigned int getExactNodeBlockSize(NodeType type) const;
      unsigned int getNodeBlockNumber(NodeType type) const;

    private:

      typedef TreeNode<BaseTraverser>* (*ConvertTreeNodeAddress)(TreeNode<BaseTraverser>* adress, unsigned int offset);

      void resize(NodeIndex index);

      TreeNodeFactory<BaseTraverser> m_treeNodeFactory;

      std::vector<std::list<unsigned int>> m_freeSlots;
      std::vector<unsigned int> m_nodeSizes;
      std::vector<ConvertTreeNodeAddress> m_nodeAddressConvert;//converts the address offsets of the different node types in bytes
      std::vector<std::vector<TreeNode<BaseTraverser>*>> m_treeNodes;//NodeType: kind of nodes | Vector: each entry is the beginning of a placement new array of the specific node Type

      unsigned int m_nodeBlockSize;
    };
  }
}

#include "Utilities/Tree/TreeNodeAllocator.inl"

#endif
