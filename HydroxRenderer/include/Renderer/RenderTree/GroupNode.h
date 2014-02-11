#ifndef RGROUPNODE_H_
#define RGROUPNODE_H_

#include "Renderer/RenderTree/TreeNode.h"

namespace he
{
	namespace renderer
	{    class GroupNode : public TreeNode
    {
    public:

      GroupNode(GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
      virtual ~GroupNode();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual TreeNode* createNewNode(InsertGeometryTraverser* traverser);

      TreeNode* getFirstChild() const;

      void setFirstChild(TreeNode* firstChild);

    protected:

      TreeNode* m_firstChild;
    };
	}
}

#endif
