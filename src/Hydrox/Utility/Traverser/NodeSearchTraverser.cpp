#include "Hydrox/Utility/Traverser/NodeSearchTraverser.h"

#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

NodeSearchTraverser::NodeSearchTraverser(const std::string& nodeName) : m_nodeName(nodeName), m_discoveredNode(nullptr)
{
}

NodeSearchTraverser::~NodeSearchTraverser()
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
  return m_discoveredNode;
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