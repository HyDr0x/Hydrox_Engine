#include "SceneGraph/Scene/TreeNodeFactory.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/PrimitiveGenerator/CubeGenerator.h>

#include <DataBase/Material.h>

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

    TreeNode* TreeNodeFactory::createTreeNode(NodeType nodeType, TreeNode *adress, const TreeNode& node)
    {
      return m_createTreeNodeFunctions[nodeType](adress, node);
    }
  }
}