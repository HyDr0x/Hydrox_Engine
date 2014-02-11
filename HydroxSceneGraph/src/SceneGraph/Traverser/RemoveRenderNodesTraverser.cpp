#include "SceneGraph/Traverser/RemoveRenderNodesTraverser.h"

namespace he
{
	namespace sg
	{    RemoveRenderNodesTraverser::RemoveRenderNodesTraverser(util::EventManager& eventManager) : m_eventManager(eventManager)
    {
    }

    RemoveRenderNodesTraverser::~RemoveRenderNodesTraverser()
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(TransformNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(TransformNode *treeNode)>(util::EventManager::OnRemoveTransformNode)->execute(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(TransformNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(AnimatedTransformNode *treeNode)>(util::EventManager::OnRemoveAnimatedTransformNode)->execute(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(LODNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LODNode *treeNode)>(util::EventManager::OnRemoveLODNode)->execute(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(LODNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(AnimatedGeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(AnimatedGeoNode *treeNode)>(util::EventManager::OnRemoveAnimatedGeometryNode)->execute(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(AnimatedGeoNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(GeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(GeoNode *treeNode)>(util::EventManager::OnRemoveGeometryNode)->execute(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(GeoNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(BillboardNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(BillboardNode *treeNode)>(util::EventManager::OnRemoveBillboardNode)->execute(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(BillboardNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(ParticleNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(ParticleNode *treeNode)>(util::EventManager::OnRemoveParticleTransmitterNode)->execute(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(ParticleNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(LightNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LightNode *treeNode)>(util::EventManager::OnRemoveLightNode)->execute(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(LightNode* treeNode)
    {
    }
	}
}
