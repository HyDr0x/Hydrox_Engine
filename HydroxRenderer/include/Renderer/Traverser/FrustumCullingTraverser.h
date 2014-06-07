#ifndef FRUSTUMCULLINGTRAVERSER_H_
#define FRUSTUMCULLINGTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

namespace he
{
  namespace renderer
  {    class IRenderNode;    class FrustumCullingTraverser : public Traverser
    {
    public:

      FrustumCullingTraverser();
      virtual ~FrustumCullingTraverser();

      virtual bool preTraverse(IRenderNode* treeNode);
    };
  }
}

#endif
