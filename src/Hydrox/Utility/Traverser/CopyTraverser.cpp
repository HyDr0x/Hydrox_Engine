#include "Hydrox/Utility/Traverser/CopyTraverser.h"

#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

namespace he
{
  CopyTraverser::CopyTraverser(std::string namePrefix) : m_namePrefix(namePrefix), m_sibling(nullptr), m_rootNode(nullptr), m_parent(nullptr)
  {
  }

  CopyTraverser::~CopyTraverser()
  {
  }

  bool CopyTraverser::preTraverse(AnimatedTransformNode* treeNode)
  {
    AnimatedTransformNode *newTransformNode = (AnimatedTransformNode*)cloneGroupNode(treeNode);

    AnimatedGeoNode *oldGeoNode = treeNode->getSkinnedMesh();
    if(oldGeoNode != nullptr)
    {
      AnimatedGeoNode *newGeoNode = m_newGeoNodeTable[oldGeoNode];
      if(newGeoNode == nullptr)
      {
        m_oldGeoNodeTable[oldGeoNode].push_back(newTransformNode);
      }
      else
      {
        newTransformNode->setSkinnedMesh(newGeoNode);
      }
    }
    
    return true;
  }

  void CopyTraverser::postTraverse(AnimatedTransformNode* treeNode)
  {
    ascendingGroupNode(treeNode);
  }

  bool CopyTraverser::preTraverse(TransformNode* treeNode)
  {
    cloneGroupNode(treeNode);
    return true;
  }

  void CopyTraverser::postTraverse(TransformNode* treeNode)
  {
    ascendingGroupNode(treeNode);
  }

  bool CopyTraverser::preTraverse(LODNode* treeNode)
  {
    cloneGroupNode(treeNode);
    return true;
  }

  void CopyTraverser::postTraverse(LODNode* treeNode)
  {
    ascendingGroupNode(treeNode);
  }

  bool CopyTraverser::preTraverse(AnimatedGeoNode* treeNode)
  {
    AnimatedGeoNode *newGeoNode = (AnimatedGeoNode*)cloneTreeNode(treeNode);
    m_newGeoNodeTable[treeNode] = newGeoNode;

    std::list<AnimatedTransformNode*> transformList = m_oldGeoNodeTable[treeNode];

    for(std::list<AnimatedTransformNode*>::iterator lit = transformList.begin(); lit != transformList.end(); lit++)
    {
      (*lit)->setSkinnedMesh(newGeoNode);
    }

    m_oldGeoNodeTable.erase(treeNode);

    return false;
  }

  void CopyTraverser::postTraverse(AnimatedGeoNode* treeNode)
  {
    ascendingTreeNode(treeNode);
  }

  bool CopyTraverser::preTraverse(GeoNode* treeNode)
  {
    cloneTreeNode(treeNode);
    return false;
  }

  void CopyTraverser::postTraverse(GeoNode* treeNode)
  {
    ascendingTreeNode(treeNode);
  }

  bool CopyTraverser::preTraverse(BillboardNode* treeNode)
  {
    cloneTreeNode(treeNode);
    return false;
  }

  void CopyTraverser::postTraverse(BillboardNode* treeNode)
  {
    ascendingTreeNode(treeNode);
  }

  bool CopyTraverser::preTraverse(ParticleNode* treeNode)
  {
    cloneTreeNode(treeNode);
    return false;
  }

  void CopyTraverser::postTraverse(ParticleNode* treeNode)
  {
    ascendingTreeNode(treeNode);
  }

  bool CopyTraverser::preTraverse(LightNode* treeNode)
  {
    cloneTreeNode(treeNode);
    return false;
  }

  void CopyTraverser::postTraverse(LightNode* treeNode)
  {
    ascendingTreeNode(treeNode);
  }

  GroupNode* CopyTraverser::getCopiedRootNode()
  {
    return m_rootNode;
  }

  GroupNode* CopyTraverser::cloneGroupNode(GroupNode* treeNode)
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

    if(treeNode->getFirstChild() != nullptr)
    {
      m_childNode = true;
    }

    return newNode;
  }

  TreeNode* CopyTraverser::cloneTreeNode(TreeNode* treeNode)
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

    if(treeNode->getFirstChild() != nullptr)
    {
      m_childNode = true;
    }

    return newNode;
  }

  void CopyTraverser::ascendingGroupNode(TreeNode* treeNode)
  {
    m_childNode = false;
    m_sibling = m_parent;
    if(m_parent != nullptr)
    {
      m_parent = m_parent->getParent();
    }
  }

  void CopyTraverser::ascendingTreeNode(TreeNode* treeNode)
  {
    m_childNode = false;
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
}

