#ifndef TREENODEALLOCATOR_H_
#define TREENODEALLOCATOR_H_

#include <vector>
#include <list>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>

#include "SceneGraph/TreeNodes/TreeNode.h"

#include "SceneGraph/Scene/TreeNodeFactory.h"

namespace he
{
  namespace sg
  {
    class GroupNode;
    class AnimatedTransformNode;

    class GRAPHICAPI TreeNodeAllocator
    {
    public:

      TreeNodeAllocator(unsigned int nodeBlockSize = 256);
      TreeNodeAllocator(const TreeNodeAllocator& other);
      virtual ~TreeNodeAllocator();

      TreeNodeAllocator& operator=(TreeNodeAllocator other);

      TreeNode& insert(NodeIndex index);
      NodeIndex insert(const TreeNode& treeNode);
      TreeNode& insert(const TreeNode& treeNode, NodeIndex index);
      void erase();
      void erase(NodeIndex index);

      TreeNode& operator[](NodeIndex index);
      const TreeNode& operator[](NodeIndex index) const;

      bool contains(NodeIndex index) const;

      void reorderTreeNodes();

      unsigned int getNodeBlockSize() const;
      unsigned int getNodeBlockNumber(NodeType type) const;

    private:

      typedef TreeNode* (*ConvertTreeNodeAddress)(TreeNode* adress, unsigned int offset);

      void resize(NodeIndex index);

      TreeNodeFactory m_treeNodeFactory;

      std::vector<std::list<unsigned int>> m_freeSlots;
      std::vector<unsigned int> m_nodeSizes;
      std::vector<ConvertTreeNodeAddress> m_nodeAddressConvert;//converts the address offsets of the different node types in bytes
      std::vector<std::vector<TreeNode*>> m_treeNodes;//NodeType: kind of nodes | Vector: each entry is the beginning of a placement new array of the specific node Type

      unsigned int m_nodeBlockSize;
    };
  }
}

#endif
