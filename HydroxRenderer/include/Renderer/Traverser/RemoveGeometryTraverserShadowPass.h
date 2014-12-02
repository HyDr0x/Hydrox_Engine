#ifndef REMOVEGEOMETRYTRAVERSERSHADOWPASS_H_
#define REMOVEGEOMETRYTRAVERSERSHADOWPASS_H_

#include "Renderer/Traverser/RemoveGeometryTraverser.h"

namespace he
{
  namespace xBar  {    class IGeometryContainer;  }

  namespace renderer
  {    class RemoveGeometryTraverserShadowPass : public RemoveGeometryTraverser
    {
    public:

      RemoveGeometryTraverserShadowPass(
        util::SingletonManager *singletonManager, 
        const xBar::IGeometryContainer& geometryContainer, 
        util::ResourceHandle staticShadowMapGenerationShaderHandle,
        util::ResourceHandle skinnedShadowMapGenerationShaderHandle);

      virtual ~RemoveGeometryTraverserShadowPass();

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

    };
  }
}

#endif
