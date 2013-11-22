#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

AnimatedGeoNode::AnimatedGeoNode(ResourceHandle meshIndex, bool renderable, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : GeoNode(meshIndex, renderable, nodeName, parent, nextSibling)
{
}

AnimatedGeoNode& AnimatedGeoNode::operator=(const AnimatedGeoNode& sourceNode)
{
  GeoNode::operator=(sourceNode);
  return *this;
}

TreeNode& AnimatedGeoNode::operator=(const TreeNode& sourceNode)
{
  assert(typeid(*this) == typeid(sourceNode));

  const AnimatedGeoNode& copyNode = static_cast<const AnimatedGeoNode&>(sourceNode);
  GeoNode::operator=(copyNode);

  m_trfMatrix = copyNode.m_trfMatrix;
  m_meshIndex = copyNode.m_meshIndex;
  m_renderable = copyNode.m_renderable;

  return *this;
}

AnimatedGeoNode::~AnimatedGeoNode()
{
}

TreeNode* AnimatedGeoNode::clone() const
{
  AnimatedGeoNode *newNode = new AnimatedGeoNode(~0, true, m_nodeName);

  newNode->m_nodeName = m_nodeName;

  newNode->m_trfMatrix = m_trfMatrix;
  newNode->m_renderable = m_renderable;
  newNode->m_meshIndex = m_meshIndex;

  return newNode;
}

bool AnimatedGeoNode::ascendTraverse(Traverser* traverser)
{
  return traverser->ascendTraverse(this);
}

bool AnimatedGeoNode::preTraverse(Traverser* traverser)
{
  return traverser->preTraverse(this);
}

void AnimatedGeoNode::postTraverse(Traverser* traverser)
{
  traverser->postTraverse(this);
}