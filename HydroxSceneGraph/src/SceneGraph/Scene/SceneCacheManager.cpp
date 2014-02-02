#include "SceneGraph/Scene/SceneCacheManager.h"

#include <assert.h>

#include "SceneGraph/Traverser/CollectRenderNodesTraverser.h"
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
	{
    SceneCacheManager::SceneCacheManager(util::EventManager& eventManager) : m_eventManager(eventManager)
    {
    }

    SceneCacheManager::~SceneCacheManager()
    {
    }

    void SceneCacheManager::setLODRanges(std::vector<float> lodRanges)
    {
      assert(lodRanges.size() > 1 && lodRanges[0] <= lodRanges[1]);

      m_lodRanges = lodRanges;
    }

    void SceneCacheManager::updateCaches(util::Vector<float, 3>& cameraPosition, float currentTime, bool isTimeRelative)
    {
      updateAnimationTime(currentTime, isTimeRelative);
      updateTransformNodes();
      updateAnimatedTransformNodes();
      updateLODNodes(cameraPosition);
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

            addTreeToCaches((*lit)->getFirstChild(), cameraPosition);
          }
        }
        else if((*lit)->getDirtyFlag() & GroupNode::LOD_INRANGE)
        {
          (*lit)->removeDirtyFlag(GroupNode::LOD_INRANGE);

          removeTreeFromCaches((*lit)->getFirstChild());
        }
      }
    }

    void SceneCacheManager::addTreeToCaches(TreeNode *rootNode, util::Vector<float, 3>& cameraPosition)
    {
      CollectRenderNodesTraverser collectTraverser(m_eventManager, m_lodRanges, cameraPosition);
      collectTraverser.addTraverserFlag(Traverser::TRAVERSE_DEFAULT);
      collectTraverser.doTraverse(rootNode->getFirstChild());

      m_activeLODs.insert(m_activeLODs.end(), collectTraverser.getActiveLODNodes().begin(), collectTraverser.getActiveLODNodes().end());
      m_activeAnimatedTransforms.insert(m_activeAnimatedTransforms.end(), collectTraverser.getActiveAnimatedTransformNodes().begin(), collectTraverser.getActiveAnimatedTransformNodes().end());
    }

    void SceneCacheManager::removeTreeFromCaches(TreeNode *rootNode)
    {
      RemoveRenderNodesTraverser removeTraverser(m_eventManager, m_lodRanges, util::Vector<float, 3>::identity());
      removeTraverser.addTraverserFlag(Traverser::TRAVERSE_IGNORE_LODS);
      removeTraverser.doTraverse(rootNode);

      deleteFromCacheList<AnimatedTransformNode>(removeTraverser.getActiveAnimatedTransformNodes(), m_activeAnimatedTransforms);
      deleteFromCacheList<TransformNode>(removeTraverser.getActiveTransformNodes(), m_dirtyTransforms);
      deleteFromCacheList<LODNode>(removeTraverser.getActiveLODNodes(), m_activeLODs);
    }
	}
}