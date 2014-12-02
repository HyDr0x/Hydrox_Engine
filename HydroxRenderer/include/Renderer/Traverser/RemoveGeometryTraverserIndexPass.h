#ifndef REMOVEGEOMETRYTRAVERSERINDEXPASS_H_
#define REMOVEGEOMETRYTRAVERSERINDEXPASS_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace renderer
  {    class RemoveGeometryTraverserIndexPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserIndexPass(
        util::SingletonManager *singletonManager,
        const xBar::IGeometryContainer& geometryContainer,
        util::ResourceHandle staticIndexGenerationShaderHandle,
        util::ResourceHandle skinnedIndexGenerationShaderHandle);

      virtual ~RemoveGeometryTraverserIndexPass();

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

    };
  }
}

#endif
