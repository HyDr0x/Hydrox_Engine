#include "SceneGraph/Traverser/NodeSearchTraverser.h"

#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"
#include "SceneGraph/TreeNodes/ShadowLightNode.h"

#include "SceneGraph/Scene/TreeNodeAllocator.h"

namespace he
{
  namespace sg
  {    NodeSearchTraverser::NodeSearchTraverser(const TreeNodeAllocator& allocator, const std::string& nodeName) : ConstTraverser(allocator), m_nodeName(nodeName), m_discoveredNode(~0)
    {
    }

    NodeSearchTraverser::~NodeSearchTraverser()
    {
    }

    bool NodeSearchTraverser::preTraverse(const AnimatedTransformNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const AnimatedTransformNode& treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(const TransformNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const TransformNode& treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(const LODNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const LODNode& treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(const AnimatedGeoNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const AnimatedGeoNode& treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(const GeoNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const GeoNode& treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(const BillboardNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const BillboardNode& treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(const ParticleNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const ParticleNode& treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(const LightNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const LightNode& treeNode)
    {
    }

    bool NodeSearchTraverser::preTraverse(const ShadowLightNode& treeNode)
    {
      return findNode(treeNode);
    }

    void NodeSearchTraverser::postTraverse(const ShadowLightNode& treeNode)
    {
    }

    NodeIndex NodeSearchTraverser::getDiscoveredNode() const
    {
      return m_discoveredNode;
    }

    bool NodeSearchTraverser::findNode(const TreeNode& treeNode)
    {
      if(treeNode.getNodeName() == m_nodeName)
      {
        m_discoveredNode = treeNode.getNodeIndex();
        m_stopTraversal = true;//stop further searching
        return false;
      }

      return true;
    }
  }
}