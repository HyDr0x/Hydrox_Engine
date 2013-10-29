#include "Hydrox/Utility/Tree/Tree.h"

#include "Hydrox/Utility/Tree/GroupNode.h"

#include "Hydrox/Utility/Traverser/DeleteTraverser.h"
#include "Hydrox/Utility/Traverser/CopyTraverser.h"

Tree::Tree()
{
}

Tree::Tree(Tree& object)
{
  addSubTree(&object, m_rootNode, "");
}

Tree::~Tree()
{
}

GroupNode* Tree::getRootNode()
{
  return m_rootNode;
}

const GroupNode* const Tree::getRootNode() const
{
  return m_rootNode;
}

TreeNode* Tree::addParentNode(TreeNode *destinationNode, GroupNode *sourceNode)
{
  GroupNode *oldParent = destinationNode->getParent();
  GroupNode *newParent = sourceNode->clone();

  newParent->setParent(oldParent);
  newParent->setNextSibling(destinationNode->getNextSibling());
  newParent->setFirstChild(destinationNode);
  destinationNode->setParent(newParent);
  destinationNode->setNextSibling(nullptr);

  if(oldParent != nullptr)
  {
    if(oldParent->getFirstChild() == destinationNode)//if the destination node was the first child, just change the parent
    {
      oldParent->setFirstChild(newParent);
    }
    else//loop until we find the preceded sibling of the destination node and change its next sibling to the new node
    {
      TreeNode *firstSibling = oldParent->getFirstChild();
      TreeNode *oldSibling = nullptr;
      while(firstSibling != destinationNode)
      {
        oldSibling = firstSibling;
        firstSibling = firstSibling->getNextSibling();
      }

      if(oldSibling != nullptr)
      {
        oldSibling->setNextSibling(newParent);
      }
    }
  }

  return newParent;
}

TreeNode* Tree::addChildNode(GroupNode* destinationNode, TreeNode* sourceNode)
{
  TreeNode* oldFirstChild = destinationNode->getFirstChild();
  TreeNode* newFirstChild = sourceNode->clone();

  destinationNode->setFirstChild(newFirstChild);
  newFirstChild->setNextSibling(oldFirstChild);
  newFirstChild->setParent(destinationNode);

  return newFirstChild;
}

void Tree::removeNode(TreeNode *node)
{
  TreeNode *sibling = node->getNextSibling();
  GroupNode *parent = node->getParent();
  TreeNode *firstChild = node->getFirstChild();

  if(parent->getFirstChild() == node)
  {
    parent->setFirstChild(firstChild);

    if(firstChild != nullptr)
    {
      TreeNode *oldChildSibling;
      while(firstChild != nullptr)
      {
        firstChild->setParent(parent);

        oldChildSibling = firstChild;
        firstChild = firstChild->getNextSibling();
      }

      oldChildSibling->setNextSibling(sibling);
    }
  }
  else
  {
    TreeNode *parentFirstChild = parent->getFirstChild();
    TreeNode *oldParentFirstChild;
    while(parentFirstChild != node)
    {
      oldParentFirstChild = parentFirstChild;
      parentFirstChild = parentFirstChild->getNextSibling();
    }

    if(firstChild != nullptr)
    {
      oldParentFirstChild->setNextSibling(firstChild);

      TreeNode *oldChildSibling;
      while(firstChild != nullptr)
      {
        firstChild->setParent(parent);

        oldChildSibling = firstChild;
        firstChild = firstChild->getNextSibling();
      }

      oldChildSibling->setNextSibling(sibling);
    }
  }

  delete node;
  node = nullptr;
}

GroupNode* Tree::addSubTree(Tree* subTree, GroupNode* parentNode, std::string namePrefix)
{
  CopyTraverser traverser(namePrefix);
  traverser.doTraverse(subTree->getRootNode());
  GroupNode *newNode = traverser.getCopiedRootNode();

  if(parentNode != nullptr && m_rootNode != nullptr)
  {
    TreeNode *oldFirstChild = parentNode->getFirstChild();

    parentNode->setFirstChild(newNode);
    newNode->setNextSibling(oldFirstChild);
    newNode->setParent(parentNode);
  }
  else
  {
    m_rootNode = newNode;
  }

  return newNode;
}

void Tree::removeSubTree(TreeNode* rootNode)
{
  GroupNode* parentNode = rootNode->getParent();
  TreeNode* currentSibling = rootNode->getFirstChild();
  TreeNode* predSibling = currentSibling;

  //disconnect the subtree from the whole tree
  while(currentSibling != nullptr)
  {
    if(currentSibling == rootNode)
    {
      if(currentSibling == predSibling && parentNode != nullptr)
      {
        parentNode->setFirstChild(currentSibling->getNextSibling());
      }
      else
      {
        predSibling->setNextSibling(currentSibling->getNextSibling());
      }

      break;//leave the loop
    }

    predSibling = currentSibling;
    currentSibling = currentSibling->getNextSibling();
  }

  if(currentSibling != nullptr)
  {
    DeleteTraverser traverser;

    traverser.doTraverse(currentSibling);
  }
}