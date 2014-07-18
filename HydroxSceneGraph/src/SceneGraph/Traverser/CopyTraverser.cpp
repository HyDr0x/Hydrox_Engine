#include "SceneGraph/Traverser/CopyTraverser.h"

#include "SceneGraph/Scene/Scene.h"

#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"
#include "SceneGraph/TreeNodes/ShadowLightNode.h"

namespace he
{
  namespace sg
  {
    CopyTraverser::CopyTraverser(const TreeNodeAllocator& sourceAllocator, TreeNodeAllocator& destinationAllocator, std::string namePrefix) :
      ConstTraverser(sourceAllocator), m_destinationAllocator(destinationAllocator), m_namePrefix(namePrefix), m_sibling(~0), m_rootNode(~0), m_parent(~0)
    {
    }

    CopyTraverser::~CopyTraverser()
    {
    }

    bool CopyTraverser::preTraverse(const AnimatedTransformNode& treeNode)
    {
      NodeIndex newTransformNodeindex = cloneGroupNode(treeNode);

      NodeIndex oldGeoNodeIndex = treeNode.getSkinnedMesh();
      if(oldGeoNodeIndex != ~0)
      {
        const AnimatedGeoNode *oldGeoNode = dynamic_cast<const AnimatedGeoNode*>(&m_allocator[oldGeoNodeIndex]);
        AnimatedTransformNode *newTransformNode = dynamic_cast<AnimatedTransformNode*>(&m_destinationAllocator[newTransformNodeindex]);

        if(!m_newGeoNodeTable.count(oldGeoNode))
        {
          m_oldGeoNodeTable[oldGeoNode].push_back(newTransformNode);
        }
        else
        {
          newTransformNode->setSkinnedMesh(m_newGeoNodeTable.find(oldGeoNode)->second);
        }
      }
    
      return true;
    }

    void CopyTraverser::postTraverse(const AnimatedTransformNode& treeNode)
    {
      ascendingGroupNode(treeNode);
    }

    bool CopyTraverser::preTraverse(const TransformNode& treeNode)
    {
      cloneGroupNode(treeNode);
      return true;
    }

    void CopyTraverser::postTraverse(const TransformNode& treeNode)
    {
      ascendingGroupNode(treeNode);
    }

    bool CopyTraverser::preTraverse(const LODNode& treeNode)
    {
      cloneGroupNode(treeNode);
      return true;
    }

    void CopyTraverser::postTraverse(const LODNode& treeNode)
    {
      ascendingGroupNode(treeNode);
    }

    bool CopyTraverser::preTraverse(const AnimatedGeoNode& treeNode)
    {
      NodeIndex newGeoNodeIndex = cloneTreeNode(treeNode);
      m_newGeoNodeTable.insert(std::map<const AnimatedGeoNode*, NodeIndex>::value_type(&treeNode, newGeoNodeIndex));

      std::list<AnimatedTransformNode*> transformList = m_oldGeoNodeTable[&treeNode];

      for(std::list<AnimatedTransformNode*>::iterator lit = transformList.begin(); lit != transformList.end(); lit++)
      {
        (*lit)->setSkinnedMesh(newGeoNodeIndex);
      }

      m_oldGeoNodeTable.erase(&treeNode);

      return false;
    }

    void CopyTraverser::postTraverse(const AnimatedGeoNode& treeNode)
    {
      ascendingTreeNode(treeNode);
    }

    bool CopyTraverser::preTraverse(const GeoNode& treeNode)
    {
      cloneTreeNode(treeNode);
      return false;
    }

    void CopyTraverser::postTraverse(const GeoNode& treeNode)
    {
      ascendingTreeNode(treeNode);
    }

    bool CopyTraverser::preTraverse(const BillboardNode& treeNode)
    {
      cloneTreeNode(treeNode);
      return false;
    }

    void CopyTraverser::postTraverse(const BillboardNode& treeNode)
    {
      ascendingTreeNode(treeNode);
    }

    bool CopyTraverser::preTraverse(const ParticleNode& treeNode)
    {
      cloneTreeNode(treeNode);
      return false;
    }

    void CopyTraverser::postTraverse(const ParticleNode& treeNode)
    {
      ascendingTreeNode(treeNode);
    }

    bool CopyTraverser::preTraverse(const LightNode& treeNode)
    {
      cloneTreeNode(treeNode);
      return false;
    }

    void CopyTraverser::postTraverse(const LightNode& treeNode)
    {
      ascendingTreeNode(treeNode);
    }

    bool CopyTraverser::preTraverse(const ShadowLightNode& treeNode)
    {
      cloneTreeNode(treeNode);
      return false;
    }

    void CopyTraverser::postTraverse(const ShadowLightNode& treeNode)
    {
      ascendingTreeNode(treeNode);
    }

    NodeIndex CopyTraverser::getCopiedRootNode() const
    {
      return m_rootNode;
    }

    NodeIndex CopyTraverser::cloneGroupNode(const GroupNode& treeNode)
    {
      NodeIndex newNodeIndex = m_destinationAllocator.insert(treeNode);
      GroupNode& newNode = (GroupNode&)m_destinationAllocator[newNodeIndex];
      newNode.setNodeName(m_namePrefix + newNode.getNodeName());
      newNode.setParent(~0);
      newNode.setFirstChild(~0);
      newNode.setNextSibling(~0);

      if(m_rootNode == ~0)//we ve got the root node
      {
        m_rootNode = newNodeIndex;
      }
      else
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

      m_sibling = newNodeIndex;

      if(treeNode.getFirstChild() != ~0)
      {
        m_parent = newNodeIndex;
        m_childNode = true;
      }

      return newNodeIndex;
    }

    NodeIndex CopyTraverser::cloneTreeNode(const TreeNode& treeNode)
    {
      NodeIndex newNodeIndex = m_destinationAllocator.insert(treeNode);
      TreeNode& newNode = m_destinationAllocator[newNodeIndex];
      newNode.setNodeName(m_namePrefix + newNode.getNodeName());

      if(m_childNode)
      {
        addChild(m_parent, newNode);
      }
      else
      {
        addSibling(m_sibling, newNode);
      }

      m_sibling = newNodeIndex;

      return newNodeIndex;
    }

    void CopyTraverser::ascendingGroupNode(const GroupNode& treeNode)
    {
      m_childNode = false;

      if(treeNode.getNextSibling() == ~0)
      {
        m_sibling = m_parent;
        if(m_parent != ~0)
        {
          m_parent = m_destinationAllocator[m_parent].getParent();
        }
      }
    }

    void CopyTraverser::ascendingTreeNode(const TreeNode& treeNode)
    {
      m_childNode = false;

      if(treeNode.getNextSibling() == ~0)
      {
        m_sibling = m_parent;
        if(m_parent != ~0)
        {
          m_parent = m_destinationAllocator[m_parent].getParent();
        }
      }
    }

    void CopyTraverser::addChild(NodeIndex parentIndex, TreeNode& newNode)
    {
      GroupNode& parent = dynamic_cast<GroupNode&>(m_destinationAllocator[parentIndex]);
      NodeIndex& oldFirstChild = parent.getFirstChild();

      newNode.setNextSibling(oldFirstChild);
      newNode.setParent(parentIndex);

      parent.setFirstChild(newNode.getNodeIndex());
    }

    void CopyTraverser::addSibling(NodeIndex siblingIndex, TreeNode& newNode)
    {
      TreeNode& sibling = m_destinationAllocator[siblingIndex];
      NodeIndex oldSiblingIndex = sibling.getNextSibling();

      newNode.setNextSibling(oldSiblingIndex);
      newNode.setParent(sibling.getParent());

      sibling.setNextSibling(newNode.getNodeIndex());
    }
  }
}

