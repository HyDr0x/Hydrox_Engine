#ifndef TREENODEFACTORY_H_
#define TREENODEFACTORY_H_

#include <map>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <SceneGraph/TreeNodes/TreeNode.h>

namespace he
{
  namespace loader
  {
    class TreeNodeFactory
    {
    public:

      typedef sg::TreeNode* (*CreateTreeNode)(std::istream stream);

      void registerCreateFunction(sg::NodeType nodeType, CreateTreeNode foo);

      CreateTreeNode createTreeNode(sg::NodeType nodeType);

    private:

      std::map<sg::NodeType, CreateTreeNode> m_createTreeNodeFunctions;
    };
  }
}

#endif
