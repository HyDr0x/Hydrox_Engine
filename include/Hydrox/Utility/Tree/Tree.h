#ifndef TREE_H_
#define TREE_H_

#include <vector>
#include <stack>

#include "Hydrox/DLLExport.h"

class TreeNode;
class GroupNode;

class GRAPHICAPI Tree
{
public:

  Tree();
  Tree(Tree& object);
  virtual ~Tree() = 0;

  GroupNode* getRootNode();
  const GroupNode* const getRootNode() const;

  virtual TreeNode* addParentNode(TreeNode *destinationNode, GroupNode *sourceNode);
  virtual TreeNode* addChildNode(GroupNode *destinationNode, TreeNode *sourceNode);
  virtual void removeNode(TreeNode *node);

  virtual GroupNode* addSubTree(Tree* subTree, GroupNode *parentNode, std::string namePrefix);
  virtual void removeSubTree(TreeNode *rootNode);

protected:

  GroupNode *m_rootNode;//the root node
};

#endif
