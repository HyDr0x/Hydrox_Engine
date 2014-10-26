#include "Utilities/Tree/TreeNode.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> void TreeNodeFactory<BaseTraverser>::registerCreateFunction(NodeType nodeType, CreateTreeNode foo)
    {
      if(!m_createTreeNodeFunctions.count(nodeType))
      {
        m_createTreeNodeFunctions[nodeType] = foo;
      }
    }

    template<class BaseTraverser> void TreeNodeFactory<BaseTraverser>::registerCreateFunction(NodeType nodeType, CreateBlankTreeNode foo)
    {
      if(!m_createBlankTreeNodeFunctions.count(nodeType))
      {
        m_createBlankTreeNodeFunctions[nodeType] = foo;
      }
    }

    template<class BaseTraverser> void TreeNodeFactory<BaseTraverser>::createTreeNode(NodeType nodeType, TreeNode<BaseTraverser> *adress, const TreeNode<BaseTraverser>& node)
    {
      m_createTreeNodeFunctions[nodeType](adress, node);
    }

    template<class BaseTraverser> void TreeNodeFactory<BaseTraverser>::createTreeNode(NodeType nodeType, TreeNode<BaseTraverser> *adress)
    {
      m_createBlankTreeNodeFunctions[nodeType](adress);
    }
  }
}