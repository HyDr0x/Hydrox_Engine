#ifndef RTREENODE_H_
#define RTREENODE_H_

#include <string>

#include <Utilities/Pointer/SharedPointer.h>

namespace he
{
  namespace renderer
  {
    class Traverser;
    class ConstTraverser;
    class InsertShadowGeometryTraverser;
    class GroupNode;

    class TreeNode
    {
    public:

      TreeNode(GroupNode * parent = nullptr, util::SharedPointer<TreeNode> nextSibling = util::SharedPointer<TreeNode>());
      virtual ~TreeNode() = 0;

      virtual bool preTraverse(Traverser* traverser) = 0;
      virtual void postTraverse(Traverser* traverser) = 0;

      virtual bool preTraverse(ConstTraverser* traverser) const = 0;
      virtual void postTraverse(ConstTraverser* traverser) const = 0;

      virtual util::SharedPointer<TreeNode> getFirstChild() const;
      util::SharedPointer<TreeNode> getNextSibling() const;
      GroupNode* getParent() const;

      void setNextSibling(util::SharedPointer<TreeNode> nextSibling);
      void setParent(GroupNode *parent);

    protected:

      GroupNode *m_parent;
      util::SharedPointer<TreeNode> m_nextSibling;

    private:

      TreeNode(const TreeNode&);
    };
  }
}

#endif
