#ifndef TREENODEFACTORY_H_
#define TREENODEFACTORY_H_

#include <map>

#include "SceneGraph/DLLExport.h"

#include <SceneGraph/TreeNodes/TreeNode.h>

namespace he
{
  namespace sg
  {
    class GRAPHICAPI TreeNodeFactory
    {
    public:

      typedef void (*CreateTreeNode)(TreeNode *adress, const TreeNode& node);
      typedef void (*CreateBlankTreeNode)(TreeNode *adress);

      void registerCreateFunction(NodeType nodeType, CreateTreeNode foo);
      void registerCreateFunction(NodeType nodeType, CreateBlankTreeNode foo);

      void createTreeNode(NodeType nodeType, TreeNode *adress, const TreeNode& node);
      void createTreeNode(NodeType nodeType, TreeNode *adress);

    private:

      std::map<NodeType, CreateTreeNode> m_createTreeNodeFunctions;
      std::map<NodeType, CreateBlankTreeNode> m_createBlankTreeNodeFunctions;
    };
  }
}

#endif
