#ifndef CONSTTRAVERSERRENDERER_H_
#define CONSTTRAVERSERRENDERER_H_

#include <Utilities/Pointer/SharedPointer.h>

#include "Renderer/DLLExport.h"

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

    class GRAPHICAPI ConstTraverser
    {
    public:

      ConstTraverser();
      virtual ~ConstTraverser() = 0;

      virtual void doTraverse(util::SharedPointer<const TreeNode> treeNode);//starts with the given tree node, but without its siblings

      virtual bool preTraverse(const TreeNode *treeNode);
      virtual void postTraverse(const TreeNode *treeNode);

      virtual bool preTraverse(const GroupNode *treeNode);
      virtual void postTraverse(const GroupNode *treeNode);

      virtual bool preTraverse(const VertexDeclarationNode *treeNode);
      virtual void postTraverse(const VertexDeclarationNode *treeNode);

      virtual bool preTraverse(const ShaderNode *treeNode);
      virtual void postTraverse(const ShaderNode *treeNode);

      virtual bool preTraverse(const TextureNode *treeNode);
      virtual void postTraverse(const TextureNode *treeNode);

      virtual bool preTraverse(const RenderNode *treeNode);
      virtual void postTraverse(const RenderNode *treeNode);

    protected:

      void doTraverseDown(util::SharedPointer<const TreeNode>treeNode);

      bool m_stopTraversal;
    };
  }
}

#endif
