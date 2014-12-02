#ifndef INSERTGEOMETRYTRAVERSERREFLECTIVESHADOWPASS_H_
#define INSERTGEOMETRYTRAVERSERREFLECTIVESHADOWPASS_H_

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class InsertGeometryTraverserReflectiveShadowPass : public InsertGeometryTraverser
    {
    public:

      InsertGeometryTraverserReflectiveShadowPass(
        util::SharedPointer<IRenderGroup> sharedRenderGroup,
        const xBar::IGeometryContainer& geometryContainer,
        util::SingletonManager *singletonManager);

      virtual ~InsertGeometryTraverserReflectiveShadowPass();

      virtual bool preTraverse(RenderNode* treeNode);

    protected:

      void createNewChildNode(VertexDeclarationNode* parent);
      void createNewSibling(RenderNode* sibling);

      util::SharedPointer<IRenderGroup> m_sharedRenderGroup;
    };
  }
}

#endif
