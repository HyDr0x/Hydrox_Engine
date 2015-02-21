#ifndef REMOVEGEOMETRYTRAVERSERREFLECTIVESHADOW_H_
#define REMOVEGEOMETRYTRAVERSERREFLECTIVESHADOW_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace xBar  {    class IGeometryContainer;  }

  namespace renderer
  {    class RemoveGeometryTraverserReflectiveShadowPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserReflectiveShadowPass();

      virtual ~RemoveGeometryTraverserReflectiveShadowPass();

      virtual void removeGeometry(TreeNode *rootNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

    };
  }
}

#endif
