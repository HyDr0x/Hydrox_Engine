#include "SceneGraph/Traverser/RemoveNodesTraverser.h"

namespace he
{
	namespace sg
	{    RemoveNodesTraverser::RemoveNodesTraverser(util::EventManager& eventManager) : m_eventManager(eventManager)
    {
    }

    RemoveNodesTraverser::~RemoveNodesTraverser()
    {
    }

    bool RemoveNodesTraverser::preTraverse(TransformNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(TransformNode *treeNode)>(util::EventManager::OnRemoveTransformNode)->execute(treeNode);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(TransformNode* treeNode)
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
      m_eventManager.raiseSignal<void (*)(AnimatedGeoNode *treeNode)>(util::EventManager::OnRemoveAnimatedGeometryNode)->execute(treeNode);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(AnimatedGeoNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(GeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(GeoNode *treeNode)>(util::EventManager::OnRemoveGeometryNode)->execute(treeNode);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(GeoNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(BillboardNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(BillboardNode *treeNode)>(util::EventManager::OnRemoveBillboardNode)->execute(treeNode);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(BillboardNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(ParticleNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(ParticleNode *treeNode)>(util::EventManager::OnRemoveParticleTransmitterNode)->execute(treeNode);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(ParticleNode* treeNode)
    {
    }

    bool RemoveNodesTraverser::preTraverse(LightNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LightNode *treeNode)>(util::EventManager::OnRemoveLightNode)->execute(treeNode);

      return true;
    }

    void RemoveNodesTraverser::postTraverse(LightNode* treeNode)
    {
    }
	}
}
