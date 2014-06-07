#include "SceneGraph/Traverser/RemoveNodesTraverser.h"

#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"

namespace he
{
  namespace sg
  {    RemoveNodesTraverser::RemoveNodesTraverser(util::EventManager& eventManager) : m_eventManager(eventManager)
    {
    }

    RemoveNodesTraverser::~RemoveNodesTraverser()
    {
    }

    bool RemoveNodesTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(AnimatedTransformNode *treeNode)>(util::EventManager::OnRemoveAnimatedTransformNode)->execute(treeNode);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(LODNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LODNode *treeNode)>(util::EventManager::OnRemoveLODNode)->execute(treeNode);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(LODNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(AnimatedGeoNode* treeNode)
    {
      treeNode->setRenderable(false);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(AnimatedGeoNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(GeoNode* treeNode)
    {
      treeNode->setRenderable(false);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(GeoNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(BillboardNode* treeNode)
    {
      treeNode->setRenderable(false);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(BillboardNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(ParticleNode* treeNode)
    {
      treeNode->setRenderable(false);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(ParticleNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(LightNode* treeNode)
    {
      treeNode->setRenderable(false);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(LightNode* treeNode)
    {
    }
  }
}
