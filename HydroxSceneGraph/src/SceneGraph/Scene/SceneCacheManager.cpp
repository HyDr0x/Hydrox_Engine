#include "SceneGraph/Scene/SceneCacheManager.h"

#include <assert.h>

#include "SceneGraph/Traverser/AddRenderNodesTraverser.h"
#include "SceneGraph/Traverser/RemoveRenderNodesTraverser.h"
#include "SceneGraph/Traverser/TransformTraverser.h"

#include "SceneGraph/TreeNodes/TreeNode.h"
#include "SceneGraph/TreeNodes/GroupNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"

namespace he
{
	namespace sg
	{    SceneCacheManager::SceneCacheManager(util::EventManager& eventManager) : m_eventManager(eventManager)
    {
      registerNodeCacheSlots();
    }

    SceneCacheManager::~SceneCacheManager()
    {
      m_dirtyTransforms.clear();
      m_activeAnimatedTransforms.clear();
      m_activeLODs.clear();
    }

    void SceneCacheManager::setLODRanges(std::vector<float> lodRanges)
    {
      assert(lodRanges.size() > 1 && lodRanges[0] <= lodRanges[1]);

      m_lodRanges = lodRanges;
    }

    void SceneCacheManager::addSubTree(TreeNode *rootNode, util::Vector<float, 3>& cameraPosition)
    {
      AddRenderNodesTraverser addRenderNodesTraverser(m_eventManager, m_lodRanges, cameraPosition);
      addRenderNodesTraverser.doTraverse(rootNode);
    }

    void SceneCacheManager::removeSubTree(TreeNode *rootNode)
    {
      RemoveRenderNodesTraverser removeRenderNodesTraverser(m_eventManager);
      removeRenderNodesTraverser.doTraverse(rootNode);
    }

    void SceneCacheManager::updateCaches(util::Vector<float, 3>& cameraPosition, float currentTime, bool isTimeRelative)
    {
      updateAnimationTime(currentTime, isTimeRelative);
      updateTransformNodes();
      updateAnimatedTransformNodes();
      updateLODNodes(cameraPosition);
    }

    void SceneCacheManager::addNodeToCache(sg::LODNode *lodNode)
    {
      m_activeLODs.push_back(lodNode);
    }

    void SceneCacheManager::addNodeToCache(sg::TransformNode *transformNode)
    {
      m_dirtyTransforms.push_back(transformNode);
    }

    void SceneCacheManager::addNodeToCache(sg::AnimatedTransformNode *animatedTransformNode)
    {
      m_activeAnimatedTransforms.push_back(animatedTransformNode);
    }

    void SceneCacheManager::removeNodeFromCache(sg::LODNode *lodNode)
    {
      m_activeLODs.remove(lodNode);
    }

    void SceneCacheManager::removeNodeFromCache(sg::TransformNode *transformNode)
    {
      m_dirtyTransforms.remove(transformNode);
    }

    void SceneCacheManager::removeNodeFromCache(sg::AnimatedTransformNode *animatedTransformNode)
    {
      m_activeAnimatedTransforms.remove(animatedTransformNode);
    }

    void SceneCacheManager::registerNodeCacheSlots()
    {
      m_eventManager.addNewSignal<void (*)(sg::LODNode *node)>(util::EventManager::OnAddLODNode);
      m_eventManager.addSlotToSignal<SceneCacheManager, void (*)(sg::LODNode *node), void (SceneCacheManager::*)(sg::LODNode *node)>(this, &SceneCacheManager::addNodeToCache, util::EventManager::OnAddLODNode);

      m_eventManager.addNewSignal<void (*)(sg::TransformNode *node)>(util::EventManager::OnAddTransformNode);
      m_eventManager.addSlotToSignal<SceneCacheManager, void (*)(sg::TransformNode *node), void (SceneCacheManager::*)(sg::TransformNode *node)>(this, &SceneCacheManager::addNodeToCache, util::EventManager::OnAddTransformNode);

      m_eventManager.addNewSignal<void (*)(sg::AnimatedTransformNode *node)>(util::EventManager::OnAddAnimatedTransformNode);
      m_eventManager.addSlotToSignal<SceneCacheManager, void (*)(sg::AnimatedTransformNode *node), void (SceneCacheManager::*)(sg::AnimatedTransformNode *node)>(this, &SceneCacheManager::addNodeToCache, util::EventManager::OnAddAnimatedTransformNode);



      m_eventManager.addNewSignal<void (*)(sg::LODNode *node)>(util::EventManager::OnRemoveLODNode);
      m_eventManager.addSlotToSignal<SceneCacheManager, void (*)(sg::LODNode *node), void (SceneCacheManager::*)(sg::LODNode *node)>(this, &SceneCacheManager::removeNodeFromCache, util::EventManager::OnRemoveLODNode);

      m_eventManager.addNewSignal<void (*)(sg::TransformNode *node)>(util::EventManager::OnRemoveTransformNode);
      m_eventManager.addSlotToSignal<SceneCacheManager, void (*)(sg::TransformNode *node), void (SceneCacheManager::*)(sg::TransformNode *node)>(this, &SceneCacheManager::removeNodeFromCache, util::EventManager::OnRemoveTransformNode);

      m_eventManager.addNewSignal<void (*)(sg::AnimatedTransformNode *node)>(util::EventManager::OnRemoveAnimatedTransformNode);
      m_eventManager.addSlotToSignal<SceneCacheManager, void (*)(sg::AnimatedTransformNode *node), void (SceneCacheManager::*)(sg::AnimatedTransformNode *node)>(this, &SceneCacheManager::removeNodeFromCache, util::EventManager::OnRemoveAnimatedTransformNode);
    }

    void SceneCacheManager::updateObserver(TransformNode* data)
    {
      m_dirtyTransforms.push_back(data);
    }

    void SceneCacheManager::updateAnimationTime(float currentTime, bool isTimeRelative)
    {
      if(isTimeRelative)
      {
        for(std::list<AnimatedTransformNode*>::iterator tit = m_activeAnimatedTransforms.begin(); tit != m_activeAnimatedTransforms.end(); tit++)
        {
          (*tit)->addCurrentAnimationTime(currentTime);   
        }
      }
      else
      {
        for(std::list<AnimatedTransformNode*>::iterator tit = m_activeAnimatedTransforms.begin(); tit != m_activeAnimatedTransforms.end(); tit++)
        {
          (*tit)->setCurrentAnimationTime(currentTime);   
        }
      }
    }

    void SceneCacheManager::updateAnimatedTransformNodes()
    {
      TransformNode *node = nullptr;
      TransformTraverser transformTraverser;

      for(std::list<AnimatedTransformNode*>::iterator tit = m_activeAnimatedTransforms.begin(); tit != m_activeAnimatedTransforms.end(); tit++)
      {
        node = *tit;
   
        if(node->getDirtyFlag() & GroupNode::TRF_DIRTY || node->getDirtyFlag() & GroupNode::ANIM_DIRTY)//traverse it only if its not been traversed before
        {
          transformTraverser.doAscend(node);//calculate the transformations of the upper path of the actual node (could be already saved in every transform node; memory / compute tradeoff)
          transformTraverser.doTraverse(node);
          transformTraverser.clearStacks();
        }
      }
    }

    void SceneCacheManager::updateTransformNodes()
    {
      TransformNode *node = nullptr;
      TransformTraverser transformTraverser;

      for(std::list<TransformNode*>::iterator tit = m_dirtyTransforms.begin(); tit != m_dirtyTransforms.end(); tit++)
      {
        node = *tit;
   
        if(node->getDirtyFlag() & GroupNode::TRF_DIRTY)//traverse it only if its not been traversed before
        {
          transformTraverser.doAscend(node);//calculate the transformations of the upper path of the actual node (could be already saved in every transform node; memory / compute tradeoff)
          transformTraverser.doTraverse(node);
          transformTraverser.clearStacks();
        }
      }

      m_dirtyTransforms.clear();
    }

    void SceneCacheManager::updateLODNodes(util::Vector<float, 3>& cameraPosition)
    {
      for(std::list<LODNode*>::iterator lit = m_activeLODs.begin(); lit != m_activeLODs.end(); lit++)
      {
        bool lodInRange = (*lit)->getLOD(cameraPosition, m_lodRanges);

        if(lodInRange)
        {
          if(!((*lit)->getDirtyFlag() & GroupNode::LOD_INRANGE))//was not in range before --> we need to update it
          {
            (*lit)->addDirtyFlag(GroupNode::LOD_INRANGE);

            AddRenderNodesTraverser addTraverser(m_eventManager, m_lodRanges, cameraPosition);
            addTraverser.doTraverse(*lit);
          }
        }
        else if((*lit)->getDirtyFlag() & GroupNode::LOD_INRANGE)
        {
          (*lit)->removeDirtyFlag(GroupNode::LOD_INRANGE);

          RemoveRenderNodesTraverser removeTraverser(m_eventManager);
          removeTraverser.doTraverse(*lit);
        }
      }
    }
	}
}