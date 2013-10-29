#include "Hydrox/Utility/Traverser/DeleteTraverser.h"

#include "Hydrox/Utility/Tree/TreeNode.h"

DeleteTraverser::DeleteTraverser()
{
}

DeleteTraverser::~DeleteTraverser()
{
}

bool DeleteTraverser::preTraverse(TreeNode* treeNode)
{
  return true;
}

void DeleteTraverser::postTraverse(TreeNode* treeNode)
{
  delete treeNode;
  treeNode = nullptr;
}