#ifndef REMOVEGEOMETRYTRAVERSERRENDERPASS_H_
#define REMOVEGEOMETRYTRAVERSERRENDERPASS_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class RemoveGeometryTraverserRenderPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserRenderPass();
      virtual ~RemoveGeometryTraverserRenderPass();

      virtual void removeGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode * treeNode);
      virtual void postTraverse(RenderNode * treeNode);

    private:

    };
  }
}

#endif
