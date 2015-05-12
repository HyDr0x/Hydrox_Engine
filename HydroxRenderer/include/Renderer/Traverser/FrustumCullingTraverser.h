#ifndef FRUSTUMCULLINGTRAVERSER_H_
#define FRUSTUMCULLINGTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/ConstTraverser.h"

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace renderer
  {
    class FrustumCullingTraverser : public ConstTraverser
    {
    public:

      FrustumCullingTraverser();
      virtual ~FrustumCullingTraverser();

      virtual bool preTraverse(const RenderNode *treeNode) override;
    };
  }
}

#endif
