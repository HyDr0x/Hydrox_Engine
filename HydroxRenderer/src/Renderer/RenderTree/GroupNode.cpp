#include "Renderer/RenderTree/GroupNode.h"

#include <assert.h>

#include "Renderer/RenderTree/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    GroupNode::GroupNode(GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : TreeNode(parent, nextSibling),
                                                                                           m_firstChild(firstChild)
    {
    }

    GroupNode::~GroupNode()
    {
      m_firstChild = nullptr;
    }

    bool GroupNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void GroupNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    TreeNode* GroupNode::createNewNode(InsertGeometryTraverser* traverser)
    {
      return traverser->createNewNode(this);
    }

    TreeNode* GroupNode::getFirstChild() const
    {
      return m_firstChild;
    }

    void GroupNode::setFirstChild(TreeNode* firstChild)
    {
      m_firstChild = firstChild;
    }
	}
}