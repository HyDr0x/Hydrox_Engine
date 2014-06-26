#ifndef UPDATETRAVERSER_H_
#define UPDATETRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace renderer
  {
    class IRenderNode;

    class UpdateTraverser : public Traverser
    {
    public:

      UpdateTraverser();
      virtual ~UpdateTraverser();

      virtual bool preTraverse(IRenderNode* treeNode);
    };
  }
}

#endif