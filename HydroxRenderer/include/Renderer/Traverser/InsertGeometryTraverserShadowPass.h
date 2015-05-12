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

      InsertGeometryTraverserShadowPass();

      virtual ~InsertGeometryTraverserShadowPass();

      virtual void insertGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<IRenderGroup> sharedRenderGroup, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode * treeNode);

    protected:

      virtual void createNewChildNode(ShaderNode * parent);

      virtual void createNewChildNode(VertexDeclarationNode * parent);
      virtual void createNewSibling(RenderNode * sibling);

      util::SharedPointer<IRenderGroup> m_sharedRenderGroup;
    };
  }
}

#endif
