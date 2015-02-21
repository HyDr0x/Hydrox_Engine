#ifndef INSERTGEOMETRYTRAVERSERRENDERPASS_H_
#define INSERTGEOMETRYTRAVERSERRENDERPASS_H_

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class InsertGeometryTraverserRenderPass : public InsertGeometryTraverser
    {
    public:

      InsertGeometryTraverserRenderPass();
      virtual ~InsertGeometryTraverserRenderPass();

      virtual void insertGeometry(TreeNode *treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode* treeNode);

      util::SharedPointer<IRenderGroup> getCreatedRenderNode() const;

    protected:

      virtual void createNewChildNode(VertexDeclarationNode* parent);
      virtual void createNewSibling(RenderNode* sibling);

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<IRenderGroup> m_createdRenderGroup;
    };
  }
}

#endif
