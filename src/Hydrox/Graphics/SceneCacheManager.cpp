#include "Hydrox/Graphics/SceneCacheManager.h"

#include "Hydrox/Utility/Traverser/CullTraverser.h"
#include "Hydrox/Utility/Traverser/TransformTraverser.h"

#include "Hydrox/Utility/Tree/TreeNode.h"
#include "Hydrox/Utility/Tree/GroupNode.h"
#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

namespace he
{
  SceneCacheManager::SceneCacheManager()
  {
  }

  SceneCacheManager::~SceneCacheManager()
  {
    m_dirtyTransforms.clear();
    m_activeLODs.clear();
    m_activeAnimatedTransforms.clear();
    m_activeAnimatedGeometry.clear();
    m_activeGeometry.clear();
    m_activeBillboards.clear();
    m_activeParticles.clear();
    m_activeLights.clear();
  }

  const std::list<AnimatedGeoNode*>& SceneCacheManager::getAnimatedMeshes()
  {
    return m_activeAnimatedGeometry;
  }

  const std::list<GeoNode*>& SceneCacheManager::getMeshes()
  {
    return m_activeGeometry;
  }

  const std::list<BillboardNode*>& SceneCacheManager::getBillboards()
  {
    return m_activeBillboards;
  }

  const std::list<LightNode*>& SceneCacheManager::getLights()
  {
    return m_activeLights;
  }

  void SceneCacheManager::setLODRanges(std::vector<float> lodRanges)
  {
    assert(lodRanges.size() > 1 && lodRanges[0] <= lodRanges[1]);

    m_lodRanges = lodRanges;
  }

  void SceneCacheManager::updateCaches(Vector<float, 3>& cameraPosition, float currentTime)
  {
    updateAnimationTime(currentTime);
    updateTransformNodes();
    updateAnimatedTransformNodes();
    updateLODNodes(cameraPosition);
  }

  void SceneCacheManager::updateObserver(TransformNode* data)
  {
    m_dirtyTransforms.push_back(data);
  }

  void SceneCacheManager::updateAnimationTime(float currentTime)
  {
    for(std::list<AnimatedTransformNode*>::iterator tit = m_activeAnimatedTransforms.begin(); tit != m_activeAnimatedTransforms.end(); tit++)
    {
      (*tit)->addCurrentAnimationTime(currentTime);   
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

  void SceneCacheManager::updateLODNodes(Vector<float, 3>& cameraPosition)
  {
    for(std::list<LODNode*>::iterator lit = m_activeLODs.begin(); lit != m_activeLODs.end(); lit++)
    {
      bool lodInRange = (*lit)->getLOD(cameraPosition, m_lodRanges);

      if(lodInRange)
      {
        if(((*lit)->getDirtyFlag() & GroupNode::LOD_INRANGE) == 0)//was not in range before --> we need to update it
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

  void SceneCacheManager::addNodeToCaches(TreeNode *newNode)
  {
    if(addSingleNodeToCacheList<LODNode>(m_activeLODs, newNode)){}
    else if(addSingleNodeToCacheList<AnimatedTransformNode>(m_activeAnimatedTransforms, newNode)){}
    else if(addSingleNodeToCacheList<AnimatedGeoNode>(m_activeAnimatedGeometry, newNode)){}
    else if(addSingleNodeToCacheList<GeoNode>(m_activeGeometry, newNode)){}
    else if(addSingleNodeToCacheList<BillboardNode>(m_activeBillboards, newNode)){}
    else if(addSingleNodeToCacheList<ParticleNode>(m_activeParticles, newNode)){}
    else if(addSingleNodeToCacheList<LightNode>(m_activeLights, newNode)){}
  }

  void SceneCacheManager::removeNodeFromCaches(TreeNode *node)
  {
    if(removeSingleNodeFromCacheList<AnimatedTransformNode>(m_activeAnimatedTransforms, node)){}
    else if(removeSingleNodeFromCacheList<TransformNode>(m_dirtyTransforms, node)){}
    else if(removeSingleNodeFromCacheList<LODNode>(m_activeLODs, node)){}
    else if(removeSingleNodeFromCacheList<AnimatedGeoNode>(m_activeAnimatedGeometry, node)){}
    else if(removeSingleNodeFromCacheList<GeoNode>(m_activeGeometry, node)){}
    else if(removeSingleNodeFromCacheList<BillboardNode>(m_activeBillboards, node)){}
    else if(removeSingleNodeFromCacheList<ParticleNode>(m_activeParticles, node)){}
    else if(removeSingleNodeFromCacheList<LightNode>(m_activeLights, node)){}
  }

  void SceneCacheManager::addTreeToCaches(TreeNode *rootNode, Vector<float, 3>& cameraPosition)
  {
    CullTraverser cullTraverser(m_lodRanges, cameraPosition);
    cullTraverser.addTraverserFlag(Traverser::TRAVERSE_DEFAULT);
    cullTraverser.doTraverse(rootNode->getFirstChild());
       
    addToCacheList<LODNode>(cullTraverser.getActiveLODNodes(), m_activeLODs);
    addToCacheList<AnimatedTransformNode>(cullTraverser.getActiveAnimatedTransformNodes(), m_activeAnimatedTransforms);
    addToCacheList<AnimatedGeoNode>(cullTraverser.getActiveAnimatedGeoNodes(), m_activeAnimatedGeometry);
    addToCacheList<GeoNode>(cullTraverser.getActiveGeoNodes(), m_activeGeometry);
    addToCacheList<BillboardNode>(cullTraverser.getActiveBillboardNodes(), m_activeBillboards);
    addToCacheList<ParticleNode>(cullTraverser.getActiveParticleNodes(), m_activeParticles);
    addToCacheList<LightNode>(cullTraverser.getActiveLightNodes(), m_activeLights);
  }

  void SceneCacheManager::removeTreeFromCaches(TreeNode *rootNode)
  {
    CullTraverser cullTraverser(m_lodRanges, Vector<float, 3>::identity());
    cullTraverser.addTraverserFlag(Traverser::TRAVERSE_IGNORE_LODS);
    cullTraverser.doTraverse(rootNode);

    deleteFromCacheList<AnimatedTransformNode>(cullTraverser.getActiveAnimatedTransformNodes(), m_activeAnimatedTransforms);
    deleteFromCacheList<TransformNode>(cullTraverser.getActiveTransformNodes(), m_dirtyTransforms);
    deleteFromCacheList<LODNode>(cullTraverser.getActiveLODNodes(), m_activeLODs);
    deleteFromCacheList<AnimatedGeoNode>(cullTraverser.getActiveAnimatedGeoNodes(), m_activeAnimatedGeometry);
    deleteFromCacheList<GeoNode>(cullTraverser.getActiveGeoNodes(), m_activeGeometry);
    deleteFromCacheList<BillboardNode>(cullTraverser.getActiveBillboardNodes(), m_activeBillboards);
    deleteFromCacheList<ParticleNode>(cullTraverser.getActiveParticleNodes(), m_activeParticles);
    deleteFromCacheList<LightNode>(cullTraverser.getActiveLightNodes(), m_activeLights);
  }
}