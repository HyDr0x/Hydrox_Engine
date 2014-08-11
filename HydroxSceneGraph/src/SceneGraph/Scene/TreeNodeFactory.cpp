#include "SceneGraph/Scene/TreeNodeFactory.h"

namespace he
{
  namespace sg
  {
    void TreeNodeFactory::registerCreateFunction(NodeType nodeType, CreateTreeNode foo)
    {
      if(!m_createTreeNodeFunctions.count(nodeType))
      {
        m_createTreeNodeFunctions[nodeType] = foo;
      }
    }

    void TreeNodeFactory::registerCreateFunction(NodeType nodeType, CreateBlankTreeNode foo)
    {
      if(!m_createBlankTreeNodeFunctions.count(nodeType))
      {
        m_createBlankTreeNodeFunctions[nodeType] = foo;
      }
    }

    void TreeNodeFactory::createTreeNode(NodeType nodeType, TreeNode *adress, const TreeNode& node)
    {
      m_createTreeNodeFunctions[nodeType](adress, node);
    }

    void TreeNodeFactory::createTreeNode(NodeType nodeType, TreeNode *adress)
    {
      m_createBlankTreeNodeFunctions[nodeType](adress);
    }
  }
}