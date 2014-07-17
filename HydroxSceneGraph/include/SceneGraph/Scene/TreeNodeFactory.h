#ifndef TREENODEFACTORY_H_
#define TREENODEFACTORY_H_

#include <map>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <SceneGraph/TreeNodes/TreeNode.h>

namespace he
{
  namespace sg
  {
    class TreeNodeFactory
    {
    public:

      typedef TreeNode* (*CreateTreeNode)(TreeNode *adress, const TreeNode& node);

      void registerCreateFunction(NodeType nodeType, CreateTreeNode foo);

      TreeNode* createTreeNode(NodeType nodeType, TreeNode *adress, const TreeNode& node);

    private:

      std::map<NodeType, CreateTreeNode> m_createTreeNodeFunctions;
    };
  }
}

#endif
