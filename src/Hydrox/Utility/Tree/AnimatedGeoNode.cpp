#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

AnimatedGeoNode::AnimatedGeoNode(const std::vector<Matrix<float, 4>>& inverseBindPoseMatrices, ResourceHandle meshIndex, bool renderable, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) 
  : GeoNode(meshIndex, renderable, nodeName, parent, nextSibling), m_inverseBindPoseMatrices(inverseBindPoseMatrices)
{
}

AnimatedGeoNode& AnimatedGeoNode::operator=(const AnimatedGeoNode& sourceNode)
{
  GeoNode::operator=(sourceNode);

  m_inverseBindPoseMatrices = sourceNode.m_inverseBindPoseMatrices;
  m_boneTransformMatrices = sourceNode.m_boneTransformMatrices;

  return *this;
}

TreeNode& AnimatedGeoNode::operator=(const TreeNode& sourceNode)
{
  assert(typeid(*this) == typeid(sourceNode));

  const AnimatedGeoNode& copyNode = static_cast<const AnimatedGeoNode&>(sourceNode);
  AnimatedGeoNode::operator=(copyNode);

  return *this;
}

AnimatedGeoNode::~AnimatedGeoNode()
{
}

TreeNode* AnimatedGeoNode::clone() const
{
  AnimatedGeoNode *newNode = new AnimatedGeoNode(m_inverseBindPoseMatrices, m_meshIndex, m_renderable, m_nodeName);

  newNode->m_trfMatrix = m_trfMatrix;
  newNode->m_boneTransformMatrices = m_boneTransformMatrices;

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

void AnimatedGeoNode::setBoneTransform(const Matrix<float, 4>& boneTransform, unsigned int boneIndex)
{
  m_boneTransformMatrices[boneIndex] = boneTransform;
}

std::vector<Matrix<float, 4>> AnimatedGeoNode::getSkinningMatrices()
{
  std::vector<Matrix<float, 4>> skinningMatrices;
  skinningMatrices.resize(m_inverseBindPoseMatrices.size());

  for(unsigned int i = 0; i < skinningMatrices.size(); i++)
  {
    skinningMatrices[i] = m_boneTransformMatrices[i] * m_inverseBindPoseMatrices[i];
  }

  return skinningMatrices;
}