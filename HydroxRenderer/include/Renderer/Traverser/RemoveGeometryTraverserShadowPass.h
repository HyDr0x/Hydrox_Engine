#ifndef REMOVEGEOMETRYTRAVERSERSHADOWPASS_H_
#define REMOVEGEOMETRYTRAVERSERSHADOWPASS_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace xBar
  {
    class IGeometryContainer;
  }

  namespace renderer
  {
    class RemoveGeometryTraverserShadowPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserShadowPass();

      virtual ~RemoveGeometryTraverserShadowPass();

      virtual void removeGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode * treeNode);
      virtual void postTraverse(RenderNode * treeNode);

    private:

    };
  }
}

#endif
