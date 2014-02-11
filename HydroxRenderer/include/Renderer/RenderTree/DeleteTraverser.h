#ifndef RDELETETRAVERSER_H_
#define RDELETETRAVERSER_H_

#include "Renderer/RenderTree/Traverser.h"

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

      virtual bool preTraverse(AnimatedRenderNode* treeNode);
      virtual void postTraverse(AnimatedRenderNode* treeNode);

      virtual bool preTraverse(StaticRenderNode* treeNode);
      virtual void postTraverse(StaticRenderNode* treeNode);
    };
	}
}

#endif
