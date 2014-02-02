#include "SceneGraph/Traverser/RemoveRenderNodesTraverser.h"

#include "SceneGraph/TreeNodes/LODNode.h"

namespace he
{
	namespace sg
	{
    RemoveRenderNodesTraverser::RemoveRenderNodesTraverser(util::EventManager& eventManager, std::vector<float> lodRanges, util::Vector<float, 3> camPos) : m_eventManager(eventManager), m_lodRanges(lodRanges), m_camPos(camPos)
    {
    }

    RemoveRenderNodesTraverser::~RemoveRenderNodesTraverser()
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      m_dirtyAnimatedTransforms.push_back(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(TransformNode* treeNode)
    {
      m_dirtyTransforms.push_back(treeNode);

      return true;
    }

    void RemoveRenderNodesTraverser::postTraverse(TransformNode* treeNode)
    {
    }

    bool RemoveRenderNodesTraverser::preTraverse(LODNode* treeNode)
    {
      m_activeLod.push_back(treeNode);

      if((m_traverserFlags & TRAVERSE_IGNORE_LODS) == 0)
      {
        return treeNode->getLOD(m_camPos, m_lodRanges);
      }
      else
      {
        return true;
      }
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

    std::list<AnimatedTransformNode*>& RemoveRenderNodesTraverser::getActiveAnimatedTransformNodes()
    {
      return m_dirtyAnimatedTransforms;
    }

    std::list<TransformNode*>& RemoveRenderNodesTraverser::getActiveTransformNodes()
    {
      return m_dirtyTransforms;
    }

    std::list<LODNode*>& RemoveRenderNodesTraverser::getActiveLODNodes()
    {
      return m_activeLod;
    }
	}
}
