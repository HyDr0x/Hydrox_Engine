#include "SceneGraph/Traverser/AddRenderNodesTraverser.h"

#include "SceneGraph/TreeNodes/LODNode.h"

namespace he
{
	namespace sg
	{    AddRenderNodesTraverser::AddRenderNodesTraverser(util::EventManager& eventManager, std::vector<float> lodRanges, util::Vector<float, 3> camPos) : m_eventManager(eventManager), m_lodRanges(lodRanges), m_camPos(camPos)
    {
    }

    AddRenderNodesTraverser::~AddRenderNodesTraverser()
    {
    }

    bool AddRenderNodesTraverser::preTraverse(TransformNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(TransformNode *treeNode)>(util::EventManager::OnAddTransformNode)->execute(treeNode);

      return true;
    }

    void AddRenderNodesTraverser::postTraverse(TransformNode* treeNode)
    {
    }

    bool AddRenderNodesTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(AnimatedTransformNode *treeNode)>(util::EventManager::OnAddAnimatedTransformNode)->execute(treeNode);

      return true;
    }

    void AddRenderNodesTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool AddRenderNodesTraverser::preTraverse(LODNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LODNode *treeNode)>(util::EventManager::OnAddLODNode)->execute(treeNode);

      return treeNode->getLOD(m_camPos, m_lodRanges);
    }

    void AddRenderNodesTraverser::postTraverse(LODNode* treeNode)
    {
    }

    bool AddRenderNodesTraverser::preTraverse(AnimatedGeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(AnimatedGeoNode *treeNode)>(util::EventManager::OnAddAnimatedGeometryNode)->execute(treeNode);

      return true;
    }

    void AddRenderNodesTraverser::postTraverse(AnimatedGeoNode* treeNode)
    {
    }


    bool AddRenderNodesTraverser::preTraverse(GeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(GeoNode *treeNode)>(util::EventManager::OnAddGeometryNode)->execute(treeNode);

      return true;
    }

    void AddRenderNodesTraverser::postTraverse(GeoNode* treeNode)
    {
    }

    bool AddRenderNodesTraverser::preTraverse(BillboardNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(BillboardNode *treeNode)>(util::EventManager::OnAddBillboardNode)->execute(treeNode);

      return true;
    }

    void AddRenderNodesTraverser::postTraverse(BillboardNode* treeNode)
    {
    }

    bool AddRenderNodesTraverser::preTraverse(ParticleNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(ParticleNode *treeNode)>(util::EventManager::OnAddParticleTransmitterNode)->execute(treeNode);

      return true;
    }

    void AddRenderNodesTraverser::postTraverse(ParticleNode* treeNode)
    {
    }

    bool AddRenderNodesTraverser::preTraverse(LightNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LightNode *treeNode)>(util::EventManager::OnAddLightNode)->execute(treeNode);

      return true;
    }

    void AddRenderNodesTraverser::postTraverse(LightNode* treeNode)
    {
    }
	}
}
