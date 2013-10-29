#include "Hydrox/Utility/Tree/TreeNode.h"

#include <typeinfo>
#include <assert.h>

TreeNode::TreeNode(const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : m_parent(parent),
                                                                                            m_nextSibling(nextSibling),
                                                                                            m_nodeName(nodeName)
{
}

TreeNode& TreeNode::operator=(const TreeNode& destinationNode)
{
  assert(typeid(*this) == typeid(destinationNode));

  const TreeNode& copyNode = static_cast<const TreeNode&>(destinationNode);

  m_parent = copyNode.m_parent;
  m_nextSibling = copyNode.m_nextSibling;
  m_nodeName = copyNode.m_nodeName;

  return *this;
}

TreeNode::~TreeNode()
{
}

TreeNode* TreeNode::getNextSibling() const
{
  return m_nextSibling;
}

GroupNode* TreeNode::getParent() const
{
  return m_parent;
}

TreeNode* TreeNode::getFirstChild() const
{
  return nullptr;
}

void TreeNode::setNextSibling(TreeNode* nextSibling)
{
  m_nextSibling = nextSibling;
}

void TreeNode::setParent(GroupNode* parent)
{
  m_parent = parent;
}

void TreeNode::setNodeName(std::string& nodeName)
{
  m_nodeName = nodeName;
}

const std::string& TreeNode::getNodeName()
{
  return m_nodeName;
}