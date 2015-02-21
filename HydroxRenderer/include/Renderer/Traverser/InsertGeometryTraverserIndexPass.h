#ifndef INSERTGEOMETRYTRAVERSERINDEXPASS_H_
#define INSERTGEOMETRYTRAVERSERINDEXPASS_H_

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class InsertGeometryTraverserIndexPass : public InsertGeometryTraverser
    {
    public:

      InsertGeometryTraverserIndexPass();

      virtual void insertGeometry(TreeNode *rootNode, util::SharedPointer<IRenderGroup> sharedRenderGroup, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual ~InsertGeometryTraverserIndexPass();

      virtual bool preTraverse(RenderNode* treeNode);

    protected:

      void createNewChildNode(VertexDeclarationNode* parent);
      void createNewSibling(RenderNode* sibling);

      util::SharedPointer<IRenderGroup> m_sharedRenderGroup;
    };
  }
}

#endif
