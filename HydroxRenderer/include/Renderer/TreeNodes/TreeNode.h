#ifndef RTREENODE_H_
#define RTREENODE_H_

#include <string>

namespace he
{
	namespace renderer
	{    class Traverser;
    class InsertGeometryTraverser;
    class GroupNode;

    class TreeNode
    {
    public:

      TreeNode(GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      virtual ~TreeNode() = 0;

      virtual bool preTraverse(Traverser* traverser) = 0;
      virtual void postTraverse(Traverser* traverser) = 0;

      virtual TreeNode* getFirstChild() const;
      TreeNode* getNextSibling() const;
      GroupNode* getParent() const;

      void setNextSibling(TreeNode* nextSibling);
      void setParent(GroupNode* parent);

    protected:

      GroupNode* m_parent;
      TreeNode* m_nextSibling;

    private:

      TreeNode(const TreeNode&);
    };
	}
}

#endif
