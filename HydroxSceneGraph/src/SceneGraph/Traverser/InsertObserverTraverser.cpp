#include "SceneGraph/Traverser/InsertObserverTraverser.h"

#include "SceneGraph/Scene/SceneCacheManager.h"
#include <Utilities/Observer/Observer.hpp>
#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"

namespace he
{
	namespace sg
	{
    InsertObserverTraverser::InsertObserverTraverser(SceneCacheManager *sceneCacheManager) : m_sceneCacheManager(sceneCacheManager)
    {
    }

    InsertObserverTraverser::~InsertObserverTraverser()
    {
    }

    bool InsertObserverTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      treeNode->add(m_sceneCacheManager);

      return true;
    }

    void InsertObserverTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool InsertObserverTraverser::preTraverse(TransformNode* treeNode)
    {
      treeNode->add(m_sceneCacheManager);

      return true;
    }

    void InsertObserverTraverser::postTraverse(TransformNode* treeNode)
    {
    }
	}
}