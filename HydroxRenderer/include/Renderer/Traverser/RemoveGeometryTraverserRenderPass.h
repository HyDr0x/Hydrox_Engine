#ifndef REMOVEGEOMETRYTRAVERSERRENDERPASS_H_
#define REMOVEGEOMETRYTRAVERSERRENDERPASS_H_

#include <Utilities/Pointer/SharedPointer.h>
#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace renderer
  {    class RemoveGeometryTraverserRenderPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserRenderPass();

      virtual ~RemoveGeometryTraverserRenderPass();

      virtual void removeGeometry(TreeNode *rootNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

    };
  }
}

#endif
