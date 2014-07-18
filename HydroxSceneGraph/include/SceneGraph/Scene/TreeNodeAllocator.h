#ifndef TREENODEALLOCATOR_H_
#define TREENODEALLOCATOR_H_

#include <map>
#include <vector>
#include <list>
#include <string>
#include <iostream>

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

      NodeIndex insert(const TreeNode& treeNode);
      TreeNode& insert(const TreeNode& treeNode, NodeIndex index);
      void erase();
      void erase(NodeIndex index);

      TreeNode& operator[](NodeIndex index);
      const TreeNode& operator[](NodeIndex index) const;

      void reorderTreeNodes();

      void writeToStream(std::ostream& ostream) const;
      void readFromStream(std::istream& istream);

    private:

      typedef TreeNode* (*ConvertTreeNodeAddress)(TreeNode* adress, unsigned int offset);

      void resize(NodeIndex index);

      TreeNodeFactory m_treeNodeFactory;

      std::map<NodeType, std::list<unsigned int>> m_freeSlots;
      std::map<NodeType, unsigned int> m_nodeSizes;
      std::map<NodeType, ConvertTreeNodeAddress> m_nodeAddressConvert;//converts the address offsets of the different node types in bytes
      std::map<NodeType, std::vector<TreeNode*>> m_treeNodes;//NodeType: kind of nodes | Vector: each entry is the beginning of a placement new array of the specific node Type

      unsigned int m_nodeBlockSize;
    };
  }
}

#endif
