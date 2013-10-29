#include "Hydrox/Utility/Tree/TransformNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

TransformNode::TransformNode(Mat<float,4>& trfMatrix, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : GroupNode(nodeName, parent, nextSibling, firstChild)
{
  math::decomposeMatrix(trfMatrix, m_angle, m_position, m_scale);
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

  m_position = copyNode.m_position;
  m_scale = copyNode.m_scale;
  m_angle = copyNode.m_angle;

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

  newNode->m_position = m_position;
  newNode->m_scale = m_scale;
  newNode->m_angle = m_angle;

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

void TransformNode::calculateTransformation(Mat<float, 4>& trfMatrix)
{
  Mat<float, 4> tlMatrix, rtxMatrix, rtyMatrix, rtzMatrix, scMatrix;

  tlMatrix = rtxMatrix = rtyMatrix = rtzMatrix = scMatrix = Mat<float, 4>::identity();

  tlMatrix[3][0] = m_position[0];
  tlMatrix[3][1] = m_position[1];
  tlMatrix[3][2] = m_position[2];

  rtxMatrix[1][1] = rtxMatrix[2][2] = cos(m_angle[0]);
  rtxMatrix[2][1] = sin(m_angle[0]);
  rtxMatrix[1][2] = -rtxMatrix[2][1];

	rtyMatrix[0][0] = rtyMatrix[2][2] = cos(m_angle[1]);
  rtyMatrix[0][2] = sin(m_angle[1]);
  rtyMatrix[2][0] = -rtyMatrix[0][2];

	rtzMatrix[0][0] = rtzMatrix[1][1] = cos(m_angle[2]);
  rtzMatrix[1][0] = sin(m_angle[2]);
  rtzMatrix[0][1] = -rtzMatrix[1][0];

  scMatrix[0][0] = m_scale[0];
	scMatrix[1][1] = m_scale[1];
	scMatrix[2][2] = m_scale[2];

  trfMatrix *= tlMatrix * rtzMatrix * rtyMatrix * rtxMatrix * scMatrix;
}

///////////////////TRANSFORMATIONS//////////////////////////

Vec<float,3> TransformNode::getPosition()
{
	return m_position;
}

Vec<float,3> TransformNode::getRotation()
{
	return m_angle;
}

Vec<float,3> TransformNode::getScale()
{
	return m_scale;
}

void TransformNode::setPosition(float x, float y, float z)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_position[0] = x;
	m_position[1] = y;
	m_position[2] = z;
}

void TransformNode::setPosition(Vec<float,3> v)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_position = v;
}

void TransformNode::setTranslation(float x, float y, float z)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_position[0] += x;
	m_position[1] += y;
	m_position[2] += z;
}

void TransformNode::setTranslation(Vec<float,3> v)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_position += v;
}

void TransformNode::setRotationX(float angle)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_angle[0] = angle;
}

void TransformNode::setTurnX(float angle)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_angle[0] += angle;
}

void TransformNode::setRotationY(float angle)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_angle[1] = angle;
}

void TransformNode::setTurnY(float angle)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_angle[1] += angle;
}

void TransformNode::setRotationZ(float angle)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_angle[2] = angle;
}

void TransformNode::setTurnZ(float angle)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_angle[2] += angle;
}

void TransformNode::setRotationXYZ(Vec<float,3> angle)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_angle = angle;
}

void TransformNode::setTurnXYZ(Vec<float,3> angle)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_angle += angle;
}

void TransformNode::setScale(Vec<float,3> s)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_scale = s;
}

void TransformNode::setScale(float a, float b, float c)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_scale[0] = a;
	m_scale[1] = b;
	m_scale[2] = c;
}

void TransformNode::Scale(Vec<float,3> s)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_scale += s;
}

void TransformNode::Scale(float a, float b, float c)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_scale[0] += a;
	m_scale[1] += b;
	m_scale[2] += c;
}

void TransformNode::setScale(float s)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_scale[0] = s;
	m_scale[1] = s;
	m_scale[2] = s;
}

void TransformNode::Scale(float s)
{
  notify(this);
  addDirtyFlag(TRF_DIRTY);

	m_scale[0] += s;
	m_scale[1] += s;
	m_scale[2] += s;
}