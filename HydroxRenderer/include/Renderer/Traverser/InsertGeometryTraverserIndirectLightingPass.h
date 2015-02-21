#ifndef INSERTGEOMETRYTRAVERSERINDIRECTLIGHTPASS_H_
#define INSERTGEOMETRYTRAVERSERINDIRECTLIGHTPASS_H_

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class InsertGeometryTraverserIndirectLightingPass : public InsertGeometryTraverser
    {
    public:

      InsertGeometryTraverserIndirectLightingPass();
      virtual ~InsertGeometryTraverserIndirectLightingPass();

      virtual void insertGeometry(TreeNode *treeNode, util::SharedPointer<IRenderGroup> sharedRenderGroup, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode* treeNode);

    protected:

      virtual void createNewChildNode(VertexDeclarationNode* parent);
      virtual void createNewSibling(RenderNode* sibling);

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<IRenderGroup> m_sharedRenderGroup;
    };
  }
}

#endif
