#ifndef REMOVEGEOMETRYTRAVERSERRENDERPASS_H_
#define REMOVEGEOMETRYTRAVERSERRENDERPASS_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace renderer
  {    class RemoveGeometryTraverserRenderPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserRenderPass(util::SingletonManager *singletonManager, const xBar::IGeometryContainer& geometryContainer);
      virtual ~RemoveGeometryTraverserRenderPass();

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

    };
  }
}

#endif
