#ifndef UTREENODEFACTORY_H_
#define UTREENODEFACTORY_H_

#include <map>

#include "Utilities/Tree/NodeIndex.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> class TreeNode;

    template<class BaseTraverser> class TreeNodeFactory
    {
    public:

      typedef void(*CreateTreeNode)(TreeNode<BaseTraverser> *adress, const TreeNode<BaseTraverser>& node);
      typedef void(*CreateBlankTreeNode)(TreeNode<BaseTraverser> *adress);

      void registerCreateFunction(NodeType nodeType, CreateTreeNode foo);
      void registerCreateFunction(NodeType nodeType, CreateBlankTreeNode foo);

      void createTreeNode(NodeType nodeType, TreeNode<BaseTraverser> *adress, const TreeNode<BaseTraverser>& node);
      void createTreeNode(NodeType nodeType, TreeNode<BaseTraverser> *adress);

    private:

      std::map<NodeType, CreateTreeNode> m_createTreeNodeFunctions;
      std::map<NodeType, CreateBlankTreeNode> m_createBlankTreeNodeFunctions;
    };
  }
}

#include "Utilities/Tree/TreeNodeFactory.inl"

#endif
