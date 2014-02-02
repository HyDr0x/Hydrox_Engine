#ifndef INSERTOBSERVERTRAVERSER_H_
#define INSERTOBSERVERTRAVERSER_H_

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
	namespace sg
	{
    class SceneCacheManager;

    class InsertObserverTraverser : public Traverser
    {
    public:

      InsertObserverTraverser(SceneCacheManager *sceneCacheManager);
      virtual ~InsertObserverTraverser();

      virtual bool preTraverse(AnimatedTransformNode* treeNode);
      virtual void postTraverse(AnimatedTransformNode* treeNode);

      virtual bool preTraverse(TransformNode* treeNode);
      virtual void postTraverse(TransformNode* treeNode);

    protected:

      SceneCacheManager *m_sceneCacheManager;
    };
	}
}

#endif
