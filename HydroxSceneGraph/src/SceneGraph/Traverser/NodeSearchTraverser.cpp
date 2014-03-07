#include "SceneGraph/Traverser/NodeSearchTraverser.h"

#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"

namespace he
{
	namespace sg
	{    NodeSearchTraverser::NodeSearchTraverser(const std::string& nodeName) : m_nodeName(nodeName), m_discoveredNode(nullptr)
    {
    }

    NodeSearchTraverser::~NodeSearchTraverser()
    {
    }

    bool NodeSearchTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(TransformNode* treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(TransformNode* treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(LODNode* treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(LODNode* treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(AnimatedGeoNode* treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(AnimatedGeoNode* treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(GeoNode* treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(GeoNode* treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(BillboardNode* treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(BillboardNode* treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(ParticleNode* treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(ParticleNode* treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(LightNode* treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(LightNode* treeNode)
    {
    }

    TreeNode* NodeSearchTraverser::getDiscoveredNode() const
    {
      if(m_discoveredNode != nullptr)
      {
        return m_discoveredNode;
      }
      else
      {
        return nullptr;//GroupNode(m_nodeName);
      }
    }

    bool NodeSearchTraverser::findNode(TreeNode* treeNode)
    {
      if(treeNode->getNodeName() == m_nodeName)
      {
        m_discoveredNode = treeNode;
        m_stopTraversal = true;//stop further searching
        return false;
      }

      return true;
    }
	}
}