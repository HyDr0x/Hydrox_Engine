#include "SceneGraph/Traverser/InsertObserverTraverser.h"

#include <Utilities/Observer/Observer.hpp>
#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"

namespace he
{
  namespace sg
  {
    InsertObserverTraverser::InsertObserverTraverser(util::Observer<TransformNode*> *observer) : m_observer(observer)
    {
    }

    InsertObserverTraverser::~InsertObserverTraverser()
    {
    }

    bool InsertObserverTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      treeNode->add(m_observer);

      return true;
    }

    void InsertObserverTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool InsertObserverTraverser::preTraverse(TransformNode* treeNode)
    {
      treeNode->add(m_observer);

      return true;
    }

    void InsertObserverTraverser::postTraverse(TransformNode* treeNode)
    {
    }
  }
}