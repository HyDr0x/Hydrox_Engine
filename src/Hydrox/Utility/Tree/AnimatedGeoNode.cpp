#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

AnimatedGeoNode::AnimatedGeoNode(ResourceHandle meshIndex, bool renderable, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                                  m_meshIndex(meshIndex),
                                                                                                                                  m_renderable(renderable)
{
}

AnimatedGeoNode& AnimatedGeoNode::operator=(const AnimatedGeoNode& destinationNode)
{
  TreeNode::operator=(destinationNode);
  return *this;
}

TreeNode& AnimatedGeoNode::operator=(const TreeNode& destinationNode)
{
  assert(typeid(*this) == typeid(destinationNode));

  const AnimatedGeoNode& copyNode = static_cast<const AnimatedGeoNode&>(destinationNode);
  TreeNode::operator=(destinationNode);

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

ResourceHandle AnimatedGeoNode::getMeshIndex() const
{
  return m_meshIndex;
}

Matrix<float,4> AnimatedGeoNode::getTransformationMatrix() const
{
  return m_trfMatrix;
}

void AnimatedGeoNode::setTransformationMatrix(const Matrix<float,4>& trfMatrix)
{
  m_trfMatrix = trfMatrix;
}

void AnimatedGeoNode::setRenderable(bool renderable)
{
  m_renderable = renderable;
}

bool AnimatedGeoNode::getRenderable() const
{
  return m_renderable;
}

void AnimatedGeoNode::setDeleteThis(bool deleteThis)
{
  m_deleteThis = deleteThis;
}

bool AnimatedGeoNode::getDeleteThis()
{
  return m_deleteThis;
}