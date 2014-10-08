#ifndef RENDERNODE_H_
#define RENDERNODE_H_

#include <Utilities/Pointer/SharedPointer.h>

#include "Renderer/TreeNodes/TreeNode.h"

namespace he
{
  namespace renderer
  {    class IRenderGroup;    class RenderNode : public TreeNode
    {
    public:

      RenderNode(util::SharedPointer<IRenderGroup> renderGroup, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      virtual ~RenderNode();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      util::SharedPointer<IRenderGroup> getRenderGroup();
      util::SharedPointer<IRenderGroup> getRenderGroup() const;

    protected:

      util::SharedPointer<IRenderGroup> m_renderGroup;
    };
  }
}

#endif
