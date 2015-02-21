#ifndef REMOVEGEOMETRYTRAVERSERINDEXPASS_H_
#define REMOVEGEOMETRYTRAVERSERINDEXPASS_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace renderer
  {    class RemoveGeometryTraverserIndexPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserIndexPass();

      virtual ~RemoveGeometryTraverserIndexPass();

      virtual void removeGeometry(TreeNode *rootNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

    };
  }
}

#endif
