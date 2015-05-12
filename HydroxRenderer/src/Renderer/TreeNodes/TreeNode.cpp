#include "Renderer/TreeNodes/TreeNode.h"

#include <typeinfo>
#include <cassert>

namespace he
{
  namespace renderer
  {
    TreeNode::TreeNode(GroupNode *parent, util::SharedPointer<TreeNode> nextSibling) : m_parent(parent),
                                                                   m_nextSibling(nextSibling)
    {
    }

    TreeNode::~TreeNode()
    {
    }

    util::SharedPointer<TreeNode> TreeNode::getNextSibling() const
    {
      return m_nextSibling;
    }

    GroupNode * TreeNode::getParent() const
    {
      return m_parent;
    }

    util::SharedPointer<TreeNode> TreeNode::getFirstChild() const
    {
      return util::SharedPointer<TreeNode>();
    }

    void TreeNode::setNextSibling(util::SharedPointer<TreeNode> nextSibling)
    {
      m_nextSibling = nextSibling;
    }

    void TreeNode::setParent(GroupNode * parent)
    {
      m_parent = parent;
    }
  }
}