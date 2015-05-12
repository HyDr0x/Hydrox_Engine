#ifndef RGROUPNODE_H_
#define RGROUPNODE_H_

#include "Renderer/TreeNodes/TreeNode.h"

namespace he
{
  namespace renderer
  {
    class GroupNode : public TreeNode
    {
    public:

      GroupNode(GroupNode * parent = nullptr, util::SharedPointer<TreeNode> nextSibling = util::SharedPointer<TreeNode>(), util::SharedPointer<TreeNode> firstChild = util::SharedPointer<TreeNode>());
      virtual ~GroupNode();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      util::SharedPointer<TreeNode> getFirstChild() const;

      void setFirstChild(util::SharedPointer<TreeNode> firstChild);

    protected:

      util::SharedPointer<TreeNode> m_firstChild;
    };
  }
}

#endif
