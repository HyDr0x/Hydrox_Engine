#include "SceneGraph/Traverser/CollectRenderNodesTraverser.h"

#include "SceneGraph/TreeNodes/LODNode.h"

namespace he
{
	namespace sg
	{
    CollectRenderNodesTraverser::CollectRenderNodesTraverser(util::EventManager& eventManager, std::vector<float> lodRanges, util::Vector<float, 3> camPos) : m_eventManager(eventManager), m_lodRanges(lodRanges), m_camPos(camPos)
    {
    }

    CollectRenderNodesTraverser::~CollectRenderNodesTraverser()
    {
    }

    bool CollectRenderNodesTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      m_dirtyAnimatedTransforms.push_back(treeNode);

      return true;
    }

    void CollectRenderNodesTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool CollectRenderNodesTraverser::preTraverse(TransformNode* treeNode)
    {
      m_dirtyTransforms.push_back(treeNode);

      return true;
    }

    void CollectRenderNodesTraverser::postTraverse(TransformNode* treeNode)
    {
    }

    bool CollectRenderNodesTraverser::preTraverse(LODNode* treeNode)
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

    void CollectRenderNodesTraverser::postTraverse(LODNode* treeNode)
    {
    }

    bool CollectRenderNodesTraverser::preTraverse(AnimatedGeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(AnimatedGeoNode *treeNode)>(util::EventManager::OnAddAnimatedGeometryNode)->execute(treeNode);

      return true;
    }

    void CollectRenderNodesTraverser::postTraverse(AnimatedGeoNode* treeNode)
    {
    }


    bool CollectRenderNodesTraverser::preTraverse(GeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(GeoNode *treeNode)>(util::EventManager::OnAddGeometryNode)->execute(treeNode);

      return true;
    }

    void CollectRenderNodesTraverser::postTraverse(GeoNode* treeNode)
    {
    }

    bool CollectRenderNodesTraverser::preTraverse(BillboardNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(BillboardNode *treeNode)>(util::EventManager::OnAddBillboardNode)->execute(treeNode);

      return true;
    }

    void CollectRenderNodesTraverser::postTraverse(BillboardNode* treeNode)
    {
    }

    bool CollectRenderNodesTraverser::preTraverse(ParticleNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(ParticleNode *treeNode)>(util::EventManager::OnAddParticleTransmitterNode)->execute(treeNode);

      return true;
    }

    void CollectRenderNodesTraverser::postTraverse(ParticleNode* treeNode)
    {
    }

    bool CollectRenderNodesTraverser::preTraverse(LightNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LightNode *treeNode)>(util::EventManager::OnAddLightNode)->execute(treeNode);

      return true;
    }

    void CollectRenderNodesTraverser::postTraverse(LightNode* treeNode)
    {
    }

    std::list<AnimatedTransformNode*>& CollectRenderNodesTraverser::getActiveAnimatedTransformNodes()
    {
      return m_dirtyAnimatedTransforms;
    }

    std::list<TransformNode*>& CollectRenderNodesTraverser::getActiveTransformNodes()
    {
      return m_dirtyTransforms;
    }

    std::list<LODNode*>& CollectRenderNodesTraverser::getActiveLODNodes()
    {
      return m_activeLod;
    }
	}
}
