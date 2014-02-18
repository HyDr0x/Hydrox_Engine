#include "SceneGraph/Traverser/AddNodesTraverser.h"

#include "SceneGraph/TreeNodes/LODNode.h"

namespace he
{
	namespace sg
	{    AddNodesTraverser::AddNodesTraverser(util::EventManager& eventManager, std::vector<float> lodRanges, util::Vector<float, 3> camPos) : m_eventManager(eventManager), m_lodRanges(lodRanges), m_camPos(camPos)
    {
    }

    AddNodesTraverser::~AddNodesTraverser()
    {
    }

    bool AddNodesTraverser::preTraverse(TransformNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(TransformNode *treeNode)>(util::EventManager::OnAddTransformNode)->execute(treeNode);

      return true;
    }

    void AddNodesTraverser::postTraverse(TransformNode* treeNode)
    {
    }

    bool AddNodesTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(AnimatedTransformNode *treeNode)>(util::EventManager::OnAddAnimatedTransformNode)->execute(treeNode);

      return true;
    }

    void AddNodesTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
    }

    bool AddNodesTraverser::preTraverse(LODNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LODNode *treeNode)>(util::EventManager::OnAddLODNode)->execute(treeNode);

      return treeNode->getLOD(m_camPos, m_lodRanges);
    }

    void AddNodesTraverser::postTraverse(LODNode* treeNode)
    {
    }

    bool AddNodesTraverser::preTraverse(AnimatedGeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(AnimatedGeoNode *treeNode)>(util::EventManager::OnAddAnimatedGeometryNode)->execute(treeNode);

      return true;
    }

    void AddNodesTraverser::postTraverse(AnimatedGeoNode* treeNode)
    {
    }


    bool AddNodesTraverser::preTraverse(GeoNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(GeoNode *treeNode)>(util::EventManager::OnAddGeometryNode)->execute(treeNode);

      return true;
    }

    void AddNodesTraverser::postTraverse(GeoNode* treeNode)
    {
    }

    bool AddNodesTraverser::preTraverse(BillboardNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(BillboardNode *treeNode)>(util::EventManager::OnAddBillboardNode)->execute(treeNode);

      return true;
    }

    void AddNodesTraverser::postTraverse(BillboardNode* treeNode)
    {
    }

    bool AddNodesTraverser::preTraverse(ParticleNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(ParticleNode *treeNode)>(util::EventManager::OnAddParticleTransmitterNode)->execute(treeNode);

      return true;
    }

    void AddNodesTraverser::postTraverse(ParticleNode* treeNode)
    {
    }

    bool AddNodesTraverser::preTraverse(LightNode* treeNode)
    {
      m_eventManager.raiseSignal<void (*)(LightNode *treeNode)>(util::EventManager::OnAddLightNode)->execute(treeNode);

      return true;
    }

    void AddNodesTraverser::postTraverse(LightNode* treeNode)
    {
    }
	}
}
