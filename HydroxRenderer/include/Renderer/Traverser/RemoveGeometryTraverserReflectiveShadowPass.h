#ifndef REMOVEGEOMETRYTRAVERSERREFLECTIVESHADOW_H_
#define REMOVEGEOMETRYTRAVERSERREFLECTIVESHADOW_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace xBar

  namespace renderer
  {
    {
    public:

      RemoveGeometryTraverserReflectiveShadowPass(
        util::SingletonManager *singletonManager, 
        const xBar::IGeometryContainer& geometryContainer);

      virtual ~RemoveGeometryTraverserReflectiveShadowPass();

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

    };
  }
}

#endif