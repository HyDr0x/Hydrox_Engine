#ifndef INSERTGEOMETRYTRAVERSERSHADOWPASS_H_
#define INSERTGEOMETRYTRAVERSERSHADOWPASS_H_

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class InsertGeometryTraverserShadowPass : public InsertGeometryTraverser
    {
    public:

      InsertGeometryTraverserShadowPass(
        util::SharedPointer<IRenderGroup> sharedRenderGroup,
        const xBar::IGeometryContainer& geometryContainer,
        util::SingletonManager *singletonManager);

      virtual ~InsertGeometryTraverserShadowPass();

      virtual bool preTraverse(RenderNode* treeNode);

    protected:

      virtual void createNewChildNode(ShaderNode* parent);

      virtual void createNewChildNode(VertexDeclarationNode* parent);
      virtual void createNewSibling(RenderNode* sibling);

      util::SharedPointer<IRenderGroup> m_sharedRenderGroup;
    };
  }
}

#endif
