#ifndef TRAVERSERRENDERER_H_
#define TRAVERSERRENDERER_H_

#include <Utilities/Pointer/SharedPointer.h>

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

    class Traverser
    {
    public:

      Traverser();
      virtual ~Traverser() = 0;

      virtual void doTraverse(util::SharedPointer<TreeNode> treeNode);//starts with the given tree node, but without its siblings

      virtual bool preTraverse(TreeNode *treeNode);
      virtual void postTraverse(TreeNode *treeNode);

      virtual bool preTraverse(GroupNode *treeNode);
      virtual void postTraverse(GroupNode *treeNode);

      virtual bool preTraverse(VertexDeclarationNode *treeNode);
      virtual void postTraverse(VertexDeclarationNode *treeNode);

      virtual bool preTraverse(ShaderNode *treeNode);
      virtual void postTraverse(ShaderNode *treeNode);

      virtual bool preTraverse(TextureNode *treeNode);
      virtual void postTraverse(TextureNode *treeNode);

      virtual bool preTraverse(RenderNode *treeNode);
      virtual void postTraverse(RenderNode *treeNode);

    protected:

      virtual void doTraverseDown(util::SharedPointer<TreeNode> treeNode);//starts with the given treeNode and traverses its siblings too

      bool m_stopTraversal;
    };
  }
}

#endif
