#include "SceneGraph/TreeNodes/TreeNode.h"

#include <typeinfo>
#include <assert.h>

namespace he
{
  namespace sg
  {
    TreeNode::TreeNode(const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) : 
      m_parent(parent),
      m_nextSibling(nextSibling),
      m_nodeName(nodeName),
      m_index(~0)
    {
      m_index.nodeType = TREENODE;
    }

    TreeNode::~TreeNode()
    {
    }

    NodeType TreeNode::getNodeType() const
    {
      return m_index.nodeType;
    }

    NodeIndex TreeNode::getNextSibling() const
    {
      return m_nextSibling;
    }

    NodeIndex TreeNode::getParent() const
    {
      return m_parent;
    }

    NodeIndex TreeNode::getFirstChild() const
    {
      return ~0;
    }

    void TreeNode::setNextSibling(NodeIndex nextSibling)
    {
      m_nextSibling = nextSibling;
    }

    void TreeNode::setParent(NodeIndex parent)
    {
      m_parent = parent;
    }

    void TreeNode::setNodeName(std::string& nodeName)
    {
      m_nodeName = nodeName;
    }

    const std::string& TreeNode::getNodeName() const
    {
      return m_nodeName;
    }

    void TreeNode::setNodeIndex(NodeIndex index)
    {
      m_index = index;
    }

    NodeIndex TreeNode::getNodeIndex() const
    {
      return m_index;
    }
  }
}