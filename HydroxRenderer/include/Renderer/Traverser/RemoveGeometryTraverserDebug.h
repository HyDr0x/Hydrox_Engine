#ifndef REMOVEGEOMETRYTRAVERSERDEBUG_H_
#define REMOVEGEOMETRYTRAVERSERDEBUG_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class RemoveGeometryTraverserDebug : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserDebug();
      virtual ~RemoveGeometryTraverserDebug();

      virtual void removeGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode * treeNode);
      virtual void postTraverse(RenderNode * treeNode);

    private:

    };
  }
}

#endif
