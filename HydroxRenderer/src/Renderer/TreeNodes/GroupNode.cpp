#include "Renderer/TreeNodes/GroupNode.h"

#include <cassert>

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Traverser/ConstTraverser.h"

namespace he
{
  namespace renderer
  {
    GroupNode::GroupNode(GroupNode * parent, util::SharedPointer<TreeNode> nextSibling, util::SharedPointer<TreeNode> firstChild) : TreeNode(parent, nextSibling),
                                                                                           m_firstChild(firstChild)
    {
    }

    GroupNode::~GroupNode()
    {
    }

    bool GroupNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void GroupNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool GroupNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void GroupNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    util::SharedPointer<TreeNode> GroupNode::getFirstChild() const
    {
      return m_firstChild;
    }

    void GroupNode::setFirstChild(util::SharedPointer<TreeNode> firstChild)
    {
      m_firstChild = firstChild;
    }
  }
}