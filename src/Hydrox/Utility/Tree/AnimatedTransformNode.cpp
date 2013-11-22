#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"
#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"

AnimatedTransformNode::AnimatedTransformNode(Matrix<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : TransformNode(trfMatrix, nodeName, parent, nextSibling, firstChild)
{
}

AnimatedTransformNode::AnimatedTransformNode(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild)
  : TransformNode(translation, scale, rotation, nodeName, parent, nextSibling, firstChild)
{
}

AnimatedTransformNode& AnimatedTransformNode::operator=(const AnimatedTransformNode& sourceNode)
{
  TransformNode::operator=(sourceNode);
  return *this;
}

TreeNode& AnimatedTransformNode::operator=(const TreeNode& sourceNode)
{
  assert(typeid(*this) == typeid(sourceNode));

  const AnimatedTransformNode& copyNode = static_cast<const AnimatedTransformNode&>(sourceNode);
  TransformNode::operator=(copyNode);

  m_animationTracks = copyNode.m_animationTracks;
  m_boneIndex = copyNode.m_boneIndex;
  m_animatedMesh = copyNode.m_animatedMesh;

  return *this;
}

AnimatedTransformNode::~AnimatedTransformNode()
{
}

GroupNode* AnimatedTransformNode::clone() const
{
  AnimatedTransformNode *newNode = new AnimatedTransformNode(Matrix<float, 4>::identity(), m_nodeName);

  newNode->m_nodeName = m_nodeName;
  newNode->m_dirtyFlag = m_dirtyFlag;

  newNode->m_translation = m_translation;
  newNode->m_scale = m_scale;
  newNode->m_rotation = m_rotation;

  newNode->m_animationTracks = m_animationTracks;
  newNode->m_boneIndex = m_boneIndex;
  newNode->m_animatedMesh = m_animatedMesh;

  return newNode;
}

bool AnimatedTransformNode::ascendTraverse(Traverser* traverser)
{
  return traverser->ascendTraverse(this);
}

bool AnimatedTransformNode::preTraverse(Traverser* traverser)
{
  return traverser->preTraverse(this);
}

void AnimatedTransformNode::postTraverse(Traverser* traverser)
{
  traverser->postTraverse(this);
}