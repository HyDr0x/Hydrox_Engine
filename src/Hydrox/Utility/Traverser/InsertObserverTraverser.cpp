#include "Hydrox/Utility/Traverser/InsertObserverTraverser.h"

#include "Hydrox/Graphics/Scene.h"
#include "Hydrox/Utility/Observer/Observer.hpp"
#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"

namespace he
{
  InsertObserverTraverser::InsertObserverTraverser(Scene *scene) : m_scene(scene)
  {
  }

  InsertObserverTraverser::~InsertObserverTraverser()
  {
  }

  bool InsertObserverTraverser::preTraverse(AnimatedTransformNode* treeNode)
  {
    treeNode->add(m_scene);

    return true;
  }

  void InsertObserverTraverser::postTraverse(AnimatedTransformNode* treeNode)
  {
  }

  bool InsertObserverTraverser::preTraverse(TransformNode* treeNode)
  {
    treeNode->add(m_scene);

    return true;
  }

  void InsertObserverTraverser::postTraverse(TransformNode* treeNode)
  {
  }
}