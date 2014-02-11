#ifndef TRAVERSER_H_
#define TRAVERSER_H_

namespace he
{
	namespace renderer
	{
    class TreeNode;
    class GroupNode;

    class VertexDeclarationNode;
    class ShaderNode;
    class TextureNode;
    class RenderNode;
    class StaticRenderNode;
    class AnimatedRenderNode;

    class Traverser
    {
    public:

      Traverser();
      virtual ~Traverser() = 0;

      virtual void doTraverse(TreeNode* treeNode);//starts with the given tree node, but without its siblings

      virtual bool preTraverse(TreeNode* treeNode);
      virtual void postTraverse(TreeNode* treeNode);

      virtual bool preTraverse(GroupNode* treeNode);
      virtual void postTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

      virtual bool preTraverse(StaticRenderNode* treeNode);
      virtual void postTraverse(StaticRenderNode* treeNode);

      virtual bool preTraverse(AnimatedRenderNode* treeNode);
      virtual void postTraverse(AnimatedRenderNode* treeNode);

    protected:

      virtual void doTraverseDown(TreeNode* treeNode);//starts with the given treeNode and traverses its siblings too

      bool m_stopTraversal;
    };
	}
}

#endif
