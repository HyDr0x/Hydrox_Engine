#include "Renderer/TreeNodes/RenderNode.h"

#include <assert.h>

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Traverser/ConstTraverser.h"

namespace he
{
  namespace renderer
  {
    RenderNode::RenderNode(util::SharedPointer<IRenderGroup> renderGroup, GroupNode* parent, TreeNode* nextSibling) : 
      TreeNode(parent, nextSibling),
      m_renderGroup(renderGroup)
    {
    }

    RenderNode::~RenderNode()
    {
    }

    bool RenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void RenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool RenderNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void RenderNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    util::SharedPointer<IRenderGroup> RenderNode::getRenderGroup()
    {
      return m_renderGroup;
    }

    util::SharedPointer<IRenderGroup> RenderNode::getRenderGroup() const
    {
      return m_renderGroup;
    }
  }
}