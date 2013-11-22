#include "Hydrox/Utility/Tree/GeoNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

GeoNode::GeoNode(ResourceHandle meshIndex, bool renderable, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                                  m_meshIndex(meshIndex),
                                                                                                                                  m_renderable(renderable)
{
}

GeoNode& GeoNode::operator=(const GeoNode& sourceNode)
{
  TreeNode::operator=(sourceNode);
  return *this;
}

TreeNode& GeoNode::operator=(const TreeNode& sourceNode)
{
  assert(typeid(*this) == typeid(sourceNode));

  const GeoNode& copyNode = static_cast<const GeoNode&>(sourceNode);
  TreeNode::operator=(copyNode);

  m_trfMatrix = copyNode.m_trfMatrix;
  m_meshIndex = copyNode.m_meshIndex;
  m_renderable = copyNode.m_renderable;

  return *this;
}

GeoNode::~GeoNode()
{
}

TreeNode* GeoNode::clone() const
{
  GeoNode *newNode = new GeoNode(~0, true, m_nodeName);

  newNode->m_nodeName = m_nodeName;

  newNode->m_trfMatrix = m_trfMatrix;
  newNode->m_renderable = m_renderable;
  newNode->m_meshIndex = m_meshIndex;

  return newNode;
}

bool GeoNode::ascendTraverse(Traverser* traverser)
{
  return traverser->ascendTraverse(this);
}

bool GeoNode::preTraverse(Traverser* traverser)
{
  return traverser->preTraverse(this);
}

void GeoNode::postTraverse(Traverser* traverser)
{
  traverser->postTraverse(this);
}

ResourceHandle GeoNode::getMeshIndex() const
{
  return m_meshIndex;
}

Matrix<float,4> GeoNode::getTransformationMatrix() const
{
  return m_trfMatrix;
}

void GeoNode::setTransformationMatrix(const Matrix<float,4>& trfMatrix)
{
  m_trfMatrix = trfMatrix;
}

void GeoNode::setRenderable(bool renderable)
{
  m_renderable = renderable;
}

bool GeoNode::getRenderable() const
{
  return m_renderable;
}