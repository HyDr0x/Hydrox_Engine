#ifndef SHADOWMAPTRAVERSER_H_
#define SHADOWMAPTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace renderer
  {
    class ShadowMapTraverser : public Traverser
    {
    public:

      ShadowMapTraverser();
      virtual ~ShadowMapTraverser();

      virtual bool preTraverse(RenderNode* treeNode);
    };
  }
}

#endif
