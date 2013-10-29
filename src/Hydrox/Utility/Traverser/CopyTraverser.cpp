#include "Hydrox/Utility/Traverser/CopyTraverser.h"

#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

CopyTraverser::CopyTraverser(std::string namePrefix) : m_namePrefix(namePrefix), m_sibling(nullptr), m_rootNode(nullptr), m_parent(nullptr)
{
}

CopyTraverser::~CopyTraverser()
{
}

void CopyTraverser::doTraverse(TreeNode* treeNode)
{
  m_childNode = true;//child node needs to be attached

  while(treeNode != nullptr)
  {
    if(treeNode->preTraverse(this))
    {
      doTraverse(treeNode->getFirstChild());
    }

    treeNode->postTraverse(this);

    if(m_stopTraversal)
    {
      return;
    }

    treeNode = treeNode->getNextSibling();
    m_childNode = false;//sibling needs to be attached
  }
}

bool CopyTraverser::preTraverse(TransformNode* treeNode)
{
  return cloneGroupNode(treeNode);
}

void CopyTraverser::postTraverse(TransformNode* treeNode)
{
  ascendingGroupNode(treeNode);
}

bool CopyTraverser::preTraverse(LODNode* treeNode)
{
  return cloneGroupNode(treeNode);
}

void CopyTraverser::postTraverse(LODNode* treeNode)
{
  ascendingGroupNode(treeNode);
}

bool CopyTraverser::preTraverse(GeoNode* treeNode)
{
  return cloneTreeNode(treeNode);
}

void CopyTraverser::postTraverse(GeoNode* treeNode)
{
  ascendingTreeNode(treeNode);
}

bool CopyTraverser::preTraverse(BillboardNode* treeNode)
{
  return cloneTreeNode(treeNode);
}

void CopyTraverser::postTraverse(BillboardNode* treeNode)
{
  ascendingTreeNode(treeNode);
}

bool CopyTraverser::preTraverse(ParticleNode* treeNode)
{
  return cloneTreeNode(treeNode);
}

void CopyTraverser::postTraverse(ParticleNode* treeNode)
{
  ascendingTreeNode(treeNode);
}

bool CopyTraverser::preTraverse(LightNode* treeNode)
{
  return cloneTreeNode(treeNode);
}

void CopyTraverser::postTraverse(LightNode* treeNode)
{
  ascendingTreeNode(treeNode);
}

GroupNode* CopyTraverser::getCopiedRootNode()
{
  return m_rootNode;
}

bool CopyTraverser::cloneGroupNode(GroupNode* treeNode)
{
  GroupNode* newNode = treeNode->clone();

  newNode->setNodeName(m_namePrefix + newNode->getNodeName());

  if(m_rootNode == nullptr)//we ve got the root node
  {
    m_rootNode = newNode;
  }
  else//we ve got a child node
  {
    if(m_childNode)
    {
      addChild(m_parent, newNode);
    }
    else
    {
      addSibling(m_sibling, newNode);
    }
  }

  m_sibling = newNode;
  m_parent = newNode;

  return true;
}

bool CopyTraverser::cloneTreeNode(TreeNode* treeNode)
{
  TreeNode* newNode = treeNode->clone();

  newNode->setNodeName(m_namePrefix + newNode->getNodeName());

  if(m_childNode)
  {
    addChild(m_parent, newNode);
  }
  else
  {
    addSibling(m_sibling, newNode);
  }

  m_sibling = newNode;

  return false;
}

void CopyTraverser::ascendingGroupNode(TreeNode* treeNode)
{
  m_sibling = m_parent;
  if(m_parent != nullptr)
  {
    m_parent = m_parent->getParent();
  }
}

void CopyTraverser::ascendingTreeNode(TreeNode* treeNode)
{
  m_sibling = m_parent;
}

void CopyTraverser::addChild(GroupNode* parent, TreeNode* newNode)
{
  TreeNode* oldFirstChild = parent->getFirstChild();

  parent->setFirstChild(newNode);
  newNode->setNextSibling(oldFirstChild);
  newNode->setParent(parent);
}

void CopyTraverser::addSibling(TreeNode* sibling, TreeNode* newNode)
{
  TreeNode* oldSibling = sibling->getNextSibling();

  sibling->setNextSibling(newNode);
  newNode->setNextSibling(oldSibling);
  newNode->setParent(sibling->getParent());
}

