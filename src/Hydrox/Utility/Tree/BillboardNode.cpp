#include "Hydrox/Utility/Tree/BillboardNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

BillboardNode::BillboardNode(ResourceHandle billboardIndex, bool renderable, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                                                    m_billboardIndex(billboardIndex),
                                                                                                                                                    m_renderable(renderable)
{
}

BillboardNode& BillboardNode::operator=(const BillboardNode& destinationNode)
{
  TreeNode::operator=(destinationNode);
  return *this;
}

TreeNode& BillboardNode::operator=(const TreeNode& destinationNode)
{
  assert(typeid(*this) == typeid(destinationNode));

  const BillboardNode& copyNode = static_cast<const BillboardNode&>(destinationNode);
  TreeNode::operator=(destinationNode);

  m_trfMatrix = copyNode.m_trfMatrix;
  m_billboardIndex = copyNode.m_billboardIndex;
  m_renderable = copyNode.m_renderable;

  return *this;
}

BillboardNode::~BillboardNode()
{
}

TreeNode* BillboardNode::clone() const
{
  BillboardNode *newNode = new BillboardNode(~0, true, m_nodeName);

  newNode->m_nodeName = m_nodeName;

  newNode->m_trfMatrix = m_trfMatrix;
  newNode->m_renderable = m_renderable;
  newNode->m_billboardIndex = m_billboardIndex;

  return newNode;
}

bool BillboardNode::ascendTraverse(Traverser* traverser)
{
  return traverser->ascendTraverse(this);
}

bool BillboardNode::preTraverse(Traverser* traverser)
{
  return traverser->preTraverse(this);
}

void BillboardNode::postTraverse(Traverser* traverser)
{
  traverser->postTraverse(this);
}

ResourceHandle BillboardNode::getBillboardIndex() const
{
  return m_billboardIndex;
}

Matrix<float,4> BillboardNode::getTransformationMatrix() const
{
  return m_trfMatrix;
}

void BillboardNode::setTransformationMatrix(const Matrix<float,4>& trfMatrix)
{
  m_trfMatrix = trfMatrix;
}

void BillboardNode::setRenderable(bool renderable)
{
  m_renderable = renderable;
}

bool BillboardNode::getRenderable() const
{
  return m_renderable;
}

void BillboardNode::setDeleteThis(bool deleteThis)
{
  m_deleteThis = deleteThis;
}

bool BillboardNode::getDeleteThis()
{
  return m_deleteThis;
}