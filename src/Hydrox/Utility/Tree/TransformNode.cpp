#include "Hydrox/Utility/Tree/TransformNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

TransformNode::TransformNode(Mat<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : GroupNode(nodeName, parent, nextSibling, firstChild)
{
  Vec<float, 3> angles;
  Vec<float, 3> scale;
  math::decomposeMatrix(trfMatrix, angles, m_translation, scale);

  assert(abs(scale[0] - scale[1]) < 0.001f && abs(scale[0] - scale[2]) < 0.001f && abs(scale[1] - scale[2]) < 0.001f);

  m_scale = scale[0];
  m_rotation = math::createRotXQuaternion(angles[0]) * math::createRotYQuaternion(angles[1]) * math::createRotZQuaternion(angles[2]);
}

TransformNode::TransformNode(Vec<float, 3>& translation, float& scale, Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild)
  : GroupNode(nodeName, parent, nextSibling, firstChild),
  m_translation(translation),
  m_scale(scale),
  m_rotation(rotation)
{
}

TransformNode& TransformNode::operator=(const TransformNode& destinationNode)
{
  TreeNode::operator=(destinationNode);
  return *this;
}

TreeNode& TransformNode::operator=(const TreeNode& destinationNode)
{
  assert(typeid(*this) == typeid(destinationNode));

  const TransformNode& copyNode = static_cast<const TransformNode&>(destinationNode);
  GroupNode::operator=(destinationNode);

  m_translation = copyNode.m_translation;
  m_scale = copyNode.m_scale;
  m_rotation = copyNode.m_rotation;

  return *this;
}

TransformNode::~TransformNode()
{
}

GroupNode* TransformNode::clone() const
{
  TransformNode *newNode = new TransformNode(Mat<float,4>::identity(), m_nodeName);

  newNode->m_nodeName = m_nodeName;
  newNode->m_dirtyFlag = m_dirtyFlag;

  newNode->m_translation = m_translation;
  newNode->m_scale = m_scale;
  newNode->m_rotation = m_rotation;

  return newNode;
}

bool TransformNode::ascendTraverse(Traverser* traverser)
{
  return traverser->ascendTraverse(this);
}

bool TransformNode::preTraverse(Traverser* traverser)
{
  return traverser->preTraverse(this);
}

void TransformNode::postTraverse(Traverser* traverser)
{
  traverser->postTraverse(this);
}

void TransformNode::calculateTransformation(Vec<float, 3>& translation, float& scale, Quaternion<float>& rotation)
{
  translation += rotation.apply(m_translation * scale);
  scale *= m_scale;
  rotation *= m_rotation;
}

///////////////////TRANSFORMATIONS//////////////////////////

Vec<float, 3> TransformNode::getPosition()
{
	return m_translation;
}

Quaternion<float> TransformNode::getRotation()
{
	return m_rotation;
}

float TransformNode::getScale()
{
	return m_scale;
}

void TransformNode::setPosition(float x, float y, float z)
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

void TransformNode::setPosition(Vec<float, 3> v)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_translation = v;
}

void TransformNode::setTranslation(float x, float y, float z)
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

void TransformNode::setTranslation(Vec<float, 3> v)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_translation += v;
}

void TransformNode::setRotation(Quaternion<float> q)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

  m_rotation = q;
}

void TransformNode::turnRotation(Quaternion<float> q)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

  m_rotation *= q;
}

void TransformNode::setRotationX(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation = math::createRotXQuaternion(angle);
}

void TransformNode::setTurnX(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation *= math::createRotXQuaternion(angle);
}

void TransformNode::setRotationY(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation = math::createRotYQuaternion(angle);
}

void TransformNode::setTurnY(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation *= math::createRotYQuaternion(angle);
}

void TransformNode::setRotationZ(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation = math::createRotZQuaternion(angle);
}

void TransformNode::setTurnZ(float angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation *= math::createRotZQuaternion(angle);
}

void TransformNode::setRotationXYZ(Vec<float, 3> angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation = math::createRotXQuaternion(angle[0]) * math::createRotYQuaternion(angle[1]) * math::createRotZQuaternion(angle[2]);
}

void TransformNode::setTurnXYZ(Vec<float, 3> angle)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_rotation *= math::createRotXQuaternion(angle[0]) * math::createRotYQuaternion(angle[1]) * math::createRotZQuaternion(angle[2]);
}

void TransformNode::setRotationAxis(float angle, Vec<float, 3> axis)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

  m_rotation = math::createRotAxisQuaternion(angle, axis);
}

void TransformNode::setTurnAxis(float angle, Vec<float, 3> axis)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

  m_rotation *= math::createRotAxisQuaternion(angle, axis);
}

void TransformNode::setScale(float s)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_scale = s;
}

void TransformNode::Scale(float s)
{
  if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
  {
    notify(this);
    addDirtyFlag(TRF_DIRTY);
  }

	m_scale += s;
}