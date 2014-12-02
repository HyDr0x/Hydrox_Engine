#ifndef REMOVEGEOMETRYTRAVERSERREFLECTIVESHADOW_H_
#define REMOVEGEOMETRYTRAVERSERREFLECTIVESHADOW_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace xBar  {    class IGeometryContainer;  }

  namespace renderer
  {    class RenderShaderContainer;    class RemoveGeometryTraverserReflectiveShadowPass : public RemoveGeometryTraverser
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
