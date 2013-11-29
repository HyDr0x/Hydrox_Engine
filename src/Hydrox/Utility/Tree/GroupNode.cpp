#include "Hydrox/Utility/Tree/GroupNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

GroupNode::GroupNode(const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                    m_firstChild(firstChild)
{
  m_dirtyFlag = DEFAULT;
}

GroupNode& GroupNode::operator=(const GroupNode& sourceNode)
{
  TreeNode::operator=(sourceNode);

  m_firstChild = sourceNode.m_firstChild;
  m_dirtyFlag = sourceNode.m_dirtyFlag;

  return *this;
}

TreeNode& GroupNode::operator=(const TreeNode& sourceNode)
{
  assert(typeid(*this) == typeid(sourceNode));

  const GroupNode& copyNode = static_cast<const GroupNode&>(sourceNode);
  GroupNode::operator=(copyNode);

  return *this;
}

GroupNode::~GroupNode()
{
  m_firstChild = nullptr;
  m_nextSibling = nullptr;
  m_parent = nullptr;
}

GroupNode* GroupNode::clone() const
{
  GroupNode *newNode = new GroupNode(m_nodeName);

  newNode->m_nodeName = m_nodeName;
  newNode->m_dirtyFlag = m_dirtyFlag;

  return newNode;
}

bool GroupNode::ascendTraverse(Traverser* traverser)
{
  return traverser->ascendTraverse(this);
}

bool GroupNode::preTraverse(Traverser* traverser)
{
  return traverser->preTraverse(this);
}

void GroupNode::postTraverse(Traverser* traverser)
{
  traverser->postTraverse(this);
}

TreeNode* GroupNode::getFirstChild() const
{
  return m_firstChild;
}

void GroupNode::setFirstChild(TreeNode* firstChild)
{
  m_firstChild = firstChild;
}

GroupNode::DirtyFlags GroupNode::getDirtyFlag() const
{
  return (DirtyFlags)m_dirtyFlag;
}

void GroupNode::addDirtyFlag(DirtyFlags dirtyFlag)
{
  m_dirtyFlag |= (unsigned int)dirtyFlag;
}

void GroupNode::removeDirtyFlag(DirtyFlags dirtyFlag)
{
  m_dirtyFlag &= ~dirtyFlag;
}