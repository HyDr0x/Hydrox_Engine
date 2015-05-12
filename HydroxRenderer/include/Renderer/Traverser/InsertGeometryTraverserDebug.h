#ifndef INSERTGEOMETRYTRAVERSERDEBUG_H_
#define INSERTGEOMETRYTRAVERSERDEBUG_H_

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class InsertGeometryTraverserDebug : public InsertGeometryTraverser
    {
    public:

      InsertGeometryTraverserDebug();
      virtual ~InsertGeometryTraverserDebug();

      virtual void insertGeometry(util::SharedPointer<TreeNode> treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode * treeNode);

    protected:

      virtual void createNewChildNode(VertexDeclarationNode * parent);
      virtual void createNewSibling(RenderNode * sibling);

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<IRenderGroup> m_createdRenderGroup;
    };
  }
}

#endif
