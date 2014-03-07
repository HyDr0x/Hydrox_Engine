#ifndef RDELETETRAVERSER_H_
#define RDELETETRAVERSER_H_

#include "Renderer/Traverser/Traverser.h"

namespace he
{
	namespace renderer
	{
    class DeleteTraverser : public Traverser
    {
    public:

      DeleteTraverser();
      virtual ~DeleteTraverser();

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      void doTraverseDown(TreeNode* treeNode);
    };
	}
}

#endif
