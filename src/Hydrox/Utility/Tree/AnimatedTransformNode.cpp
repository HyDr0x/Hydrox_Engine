#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

AnimatedTransformNode::AnimatedTransformNode(Matrix<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : GroupNode(nodeName, parent, nextSibling, firstChild)
{
  Vector<float, 3> angles;
  Vector<float, 3> scale;
  math::decomposeMatrix(trfMatrix, angles, m_translation, scale);

  assert(abs(scale[0] - scale[1]) < 0.001f && abs(scale[0] - scale[2]) < 0.001f && abs(scale[1] - scale[2]) < 0.001f);

  m_scale = scale[0];
  m_rotation = math::createRotXQuaternion(angles[0]) * math::createRotYQuaternion(angles[1]) * math::createRotZQuaternion(angles[2]);
}

AnimatedTransformNode::AnimatedTransformNode(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild)
  : GroupNode(nodeName, parent, nextSibling, firstChild),
  m_translation(translation),
  m_scale(scale),
  m_rotation(rotation)
{
}

AnimatedTransformNode& AnimatedTransformNode::operator=(const AnimatedTransformNode& destinationNode)
{
  TreeNode::operator=(destinationNode);
  return *this;
}

TreeNode& AnimatedTransformNode::operator=(const TreeNode& destinationNode)
{
  assert(typeid(*this) == typeid(destinationNode));

  const AnimatedTransformNode& copyNode = static_cast<const AnimatedTransformNode&>(destinationNode);
  GroupNode::operator=(destinationNode);

  m_translation = copyNode.m_translation;
  m_scale = copyNode.m_scale;
  m_rotation = copyNode.m_rotation;

  return *this;
}

AnimatedTransformNode::~AnimatedTransformNode()
{
}

GroupNode* AnimatedTransformNode::clone() const
{
  AnimatedTransformNode *newNode = new AnimatedTransformNode(Matrix<float,4>::identity(), m_nodeName);

  newNode->m_nodeName = m_nodeName;
  newNode->m_dirtyFlag = m_dirtyFlag;

  newNode->m_translation = m_translation;
  newNode->m_scale = m_scale;
  newNode->m_rotation = m_rotation;

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

void AnimatedTransformNode::calculateTransformation(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation)
{
  translation += rotation.apply(m_translation * scale);
  scale *= m_scale;
  rotation *= m_rotation;
}

///////////////////TRANSFORMATIONS//////////////////////////

Vector<float, 3> AnimatedTransformNode::getPosition()
{
	return m_translation;
}

Quaternion<float> AnimatedTransformNode::getRotation()
{
	return m_rotation;
}

float AnimatedTransformNode::getScale()
{
	return m_scale;
}

void AnimatedTransformNode::setPosition(float x, float y, float z)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_translation[0] = x;
	m_translation[1] = y;
	m_translation[2] = z;
}

void AnimatedTransformNode::setPosition(Vector<float, 3> v)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_translation = v;
}

void AnimatedTransformNode::setTranslation(float x, float y, float z)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_translation[0] += x;
	m_translation[1] += y;
	m_translation[2] += z;
}

void AnimatedTransformNode::setTranslation(Vector<float, 3> v)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_translation += v;
}

void AnimatedTransformNode::setRotation(Quaternion<float> q)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

  m_rotation = q;
}

void AnimatedTransformNode::turnRotation(Quaternion<float> q)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

  m_rotation *= q;
}

void AnimatedTransformNode::setRotationX(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation = math::createRotXQuaternion(angle);
}

void AnimatedTransformNode::setTurnX(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation *= math::createRotXQuaternion(angle);
}

void AnimatedTransformNode::setRotationY(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation = math::createRotYQuaternion(angle);
}

void AnimatedTransformNode::setTurnY(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation *= math::createRotYQuaternion(angle);
}

void AnimatedTransformNode::setRotationZ(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation = math::createRotZQuaternion(angle);
}

void AnimatedTransformNode::setTurnZ(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation *= math::createRotZQuaternion(angle);
}

void AnimatedTransformNode::setRotationXYZ(Vector<float, 3> angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation = math::createRotXQuaternion(angle[0]) * math::createRotYQuaternion(angle[1]) * math::createRotZQuaternion(angle[2]);
}

void AnimatedTransformNode::setTurnXYZ(Vector<float, 3> angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation *= math::createRotXQuaternion(angle[0]) * math::createRotYQuaternion(angle[1]) * math::createRotZQuaternion(angle[2]);
}

void AnimatedTransformNode::setRotationAxis(float angle, Vector<float, 3> axis)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

  m_rotation = math::createRotAxisQuaternion(angle, axis);
}

void AnimatedTransformNode::setTurnAxis(float angle, Vector<float, 3> axis)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

  m_rotation *= math::createRotAxisQuaternion(angle, axis);
}

void AnimatedTransformNode::setScale(float s)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_scale = s;
}

void AnimatedTransformNode::Scale(float s)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_scale += s;
}