#ifndef INSERTOBSERVERTRAVERSER_H_
#define INSERTOBSERVERTRAVERSER_H_

#include <Utilities/Observer/Observer.hpp>

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    class InsertObserverTraverser : public Traverser
    {
    public:

      InsertObserverTraverser(TreeNodeAllocator& allocator, util::Observer<TransformNode*> *observer);
      virtual ~InsertObserverTraverser();

      bool preTraverse(AnimatedTransformNode& treeNode);
      void postTraverse(AnimatedTransformNode& treeNode);

      bool preTraverse(TransformNode& treeNode);
      void postTraverse(TransformNode& treeNode);

    protected:

      util::Observer<TransformNode*> *m_observer;
    };
  }
}

#endif
