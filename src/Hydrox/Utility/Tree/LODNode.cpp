#include "Hydrox/Utility/Tree/LODNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

LODNode::LODNode(Vec<float, 3> position, unsigned int lodLevel, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : 
GroupNode(nodeName, parent, nextSibling, firstChild),
  m_lodLevel(lodLevel),
  m_position(position)
{
  m_dirtyFlag |= LOD_INRANGE;
  m_transformedPosition = m_position;
}

LODNode& LODNode::operator=(const LODNode& destinationNode)
{
  TreeNode::operator=(destinationNode);
  return *this;
}

TreeNode& LODNode::operator=(const TreeNode& destinationNode)
{
  assert(typeid(*this) == typeid(destinationNode));

  const LODNode& copyNode = static_cast<const LODNode&>(destinationNode);
  GroupNode::operator=(destinationNode);

  m_lodLevel = copyNode.m_lodLevel;
  m_position = copyNode.m_position;
  m_transformedPosition = copyNode.m_transformedPosition;

  return *this;
}

LODNode::~LODNode()
{
}

GroupNode* LODNode::clone() const
{
  LODNode *newNode = new LODNode(m_position, m_lodLevel, m_nodeName);

  newNode->m_nodeName = m_nodeName;
  newNode->m_dirtyFlag = m_dirtyFlag;
  newNode->m_position = m_position;
  newNode->m_transformedPosition = m_transformedPosition;

  return newNode;
}

bool LODNode::ascendTraverse(Traverser* traverser)
{
  return traverser->ascendTraverse(this);
}

bool LODNode::preTraverse(Traverser* traverser)
{
  return traverser->preTraverse(this);
}

void LODNode::postTraverse(Traverser* traverser)
{
  traverser->postTraverse(this);
}

unsigned int LODNode::getLODLevel()
{
  return m_lodLevel;
}

bool LODNode::getLOD(Vec<float, 3> camPos, const std::vector<float>& lodRanges)
{
  float distance = static_cast<float>((m_transformedPosition - camPos).length());

	return lodRanges[m_lodLevel] < distance && distance < lodRanges[std::max(m_lodLevel + 1, m_lodLevel)];
}

void LODNode::transformPosition(Vec<float, 3>& translation, Vec<float, 3>& scale, Quaternion<float>& rotation)
{
  m_transformedPosition = translation + rotation.apply(scale * m_position);
}