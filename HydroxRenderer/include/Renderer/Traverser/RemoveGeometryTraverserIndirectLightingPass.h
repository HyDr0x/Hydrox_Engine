#ifndef REMOVEGEOMETRYTRAVERSERINDIRECTLIGHTINGPASS_H_
#define REMOVEGEOMETRYTRAVERSERINDIRECTLIGHTINGPASS_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class RemoveGeometryTraverserIndirectLightingPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserIndirectLightingPass();
      virtual ~RemoveGeometryTraverserIndirectLightingPass();

      virtual void removeGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode * treeNode);
      virtual void postTraverse(RenderNode * treeNode);

    private:

    };
  }
}

#endif
