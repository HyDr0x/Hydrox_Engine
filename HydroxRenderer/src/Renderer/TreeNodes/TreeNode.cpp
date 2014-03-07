#include "Renderer/TreeNodes/TreeNode.h"

#include <typeinfo>
#include <assert.h>

namespace he
{
	namespace renderer
	{
    TreeNode::TreeNode(GroupNode* parent, TreeNode* nextSibling) : m_parent(parent),
                                                                   m_nextSibling(nextSibling)
    {
    }

    TreeNode::~TreeNode()
    {
      m_nextSibling = nullptr;
      m_parent = nullptr;
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
	}
}