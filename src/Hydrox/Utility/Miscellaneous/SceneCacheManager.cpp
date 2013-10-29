#include "Hydrox/Utility/Miscellaneous/SceneCacheManager.h"

#include "Hydrox/Utility/Traverser/CullTraverser.h"
#include "Hydrox/Utility/Traverser/TransformTraverser.h"

#include "Hydrox/Services/Camera.h"

#include "Hydrox/Utility/Tree/TreeNode.h"
#include "Hydrox/Utility/Tree/GroupNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

SceneCacheManager::SceneCacheManager()
{
  m_camera = nullptr;
}

SceneCacheManager::SceneCacheManager(Camera *camera) : m_camera(camera)
{
}

SceneCacheManager::~SceneCacheManager()
{
  m_dirtyTransforms.clear();
  m_activeLODs.clear();
  m_activeGeometry.clear();
  m_activeBillboards.clear();
  m_activeParticles.clear();
  m_activeLights.clear();
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
  m_lodRanges = lodRanges;
}

void SceneCacheManager::updateCaches()
{
  updateTransformNodes();
  updateLODNodes();
}

void SceneCacheManager::addNodeToCaches(TreeNode *newNode, NodeTypes types)
{
  if(types & TRANSFORMNODE && addSingleNodeToCacheList<TransformNode>(m_dirtyTransforms, newNode)){}
  else if(types & LODNODE && addSingleNodeToCacheList<LODNode>(m_activeLODs, newNode)){}
  else if(types & GEONODE && addSingleNodeToCacheList<GeoNode>(m_activeGeometry, newNode)){}
  else if(types & BILLBOARDNODE && addSingleNodeToCacheList<BillboardNode>(m_activeBillboards, newNode)){}
  else if(types & PARTICLENODE && addSingleNodeToCacheList<ParticleNode>(m_activeParticles, newNode)){}
  else if(types & LIGHTNODE && addSingleNodeToCacheList<LightNode>(m_activeLights, newNode)){}
}

void SceneCacheManager::removeNodeFromCaches(TreeNode *node, NodeTypes types)
{
  if(types & TRANSFORMNODE && removeSingleNodeFromCacheList<TransformNode>(m_dirtyTransforms, node)){}
  else if(types & LODNODE && removeSingleNodeFromCacheList<LODNode>(m_activeLODs, node)){}
  else if(types & GEONODE && removeSingleNodeFromCacheList<GeoNode>(m_activeGeometry, node)){}
  else if(types & BILLBOARDNODE && removeSingleNodeFromCacheList<BillboardNode>(m_activeBillboards, node)){}
  else if(types & PARTICLENODE && removeSingleNodeFromCacheList<ParticleNode>(m_activeParticles, node)){}
  else if(types & LIGHTNODE && removeSingleNodeFromCacheList<LightNode>(m_activeLights, node)){}
}

void SceneCacheManager::addTreeToCaches(TreeNode *rootNode, NodeTypes types, Traverser::TraverserFlags traverserFlag)
{
  CullTraverser cullTraverser(m_lodRanges, m_camera->getPosition());
  cullTraverser.addTraverserFlag(traverserFlag);
  cullTraverser.doTraverse(rootNode->getFirstChild());
        
  if(types & TRANSFORMNODE)
  {
    addToCacheList<TransformNode>(cullTraverser.getActiveTransformNodes(), m_dirtyTransforms);
  }

  if(types & LODNODE)
  {
    addToCacheList<LODNode>(cullTraverser.getActiveLODNodes(), m_activeLODs);
  }
  if(types & GEONODE)
  {
    addToCacheList<GeoNode>(cullTraverser.getActiveGeoNodes(), m_activeGeometry);
  }

  if(types & BILLBOARDNODE)
  {
    addToCacheList<BillboardNode>(cullTraverser.getActiveBillboardNodes(), m_activeBillboards);
  }

  if(types & PARTICLENODE)
  {
    addToCacheList<ParticleNode>(cullTraverser.getActiveParticleNodes(), m_activeParticles);
  }

  if(types & LIGHTNODE)
  {
    addToCacheList<LightNode>(cullTraverser.getActiveLightNodes(), m_activeLights);
  }
}

void SceneCacheManager::removeTreeFromCaches(TreeNode *rootNode, NodeTypes types, Traverser::TraverserFlags traverserFlag)
{
  CullTraverser cullTraverser(m_lodRanges, m_camera->getPosition());
  cullTraverser.addTraverserFlag(traverserFlag);
  cullTraverser.doTraverse(rootNode);

  if(types & TRANSFORMNODE)
  {
    deleteFromCacheList<TransformNode>(cullTraverser.getActiveTransformNodes(), m_dirtyTransforms);
  }

  if(types & LODNODE)
  {
    deleteFromCacheList<LODNode>(cullTraverser.getActiveLODNodes(), m_activeLODs);
  }
  if(types & GEONODE)
  {
    deleteFromCacheList<GeoNode>(cullTraverser.getActiveGeoNodes(), m_activeGeometry);
  }

  if(types & BILLBOARDNODE)
  {
    deleteFromCacheList<BillboardNode>(cullTraverser.getActiveBillboardNodes(), m_activeBillboards);
  }

  if(types & PARTICLENODE)
  {
    deleteFromCacheList<ParticleNode>(cullTraverser.getActiveParticleNodes(), m_activeParticles);
  }

  if(types & LIGHTNODE)
  {
    deleteFromCacheList<LightNode>(cullTraverser.getActiveLightNodes(), m_activeLights);
  }
}

void SceneCacheManager::updateObserver(TransformNode* data)
{
  m_dirtyTransforms.push_back(data);
}

void SceneCacheManager::updateTransformNodes()
{
  TransformTraverser transformTraverser;
  TransformNode *node = nullptr;

  std::stack<Mat<float, 4>> matrixStack;
  Mat<float, 4> finalTrfMatrix = Mat<float, 4>::identity();

  for(std::list<TransformNode*>::iterator tit = m_dirtyTransforms.begin(); tit != m_dirtyTransforms.end(); tit++)
  {
    node = *tit;
   
    TransformTraverser transformUpTraversal;
    transformUpTraversal.doAscend(node);

    matrixStack = transformUpTraversal.getMatrixStack();

    while(!matrixStack.empty())
    {
      finalTrfMatrix *= matrixStack.top();
      matrixStack.pop();
    }

    transformTraverser.insertTransformMatrix(finalTrfMatrix);//insert trf matrix of the upper path of the actual node (could be already saved in every transform node; memory / compute tradeoff)
    transformTraverser.doTraverse(node);
  }

  m_dirtyTransforms.clear();
}

void SceneCacheManager::updateLODNodes()
{
  for(std::list<LODNode*>::iterator lit = m_activeLODs.begin(); lit != m_activeLODs.end(); lit++)
  {
    bool lodInRange = (*lit)->getLOD(m_camera->getPosition(), m_lodRanges);

    if(lodInRange)
    {
      if(((*lit)->getDirtyFlag() & GroupNode::LOD_INRANGE) == 0)//was not in range before --> we need to update it
      {
        (*lit)->addDirtyFlag(GroupNode::LOD_INRANGE);

        addTreeToCaches((*lit)->getFirstChild(), (NodeTypes)(LODNODE | GEONODE | BILLBOARDNODE | PARTICLENODE | LIGHTNODE), Traverser::TRAVERSE_DEFAULT);
      }
    }
    else if((*lit)->getDirtyFlag() & GroupNode::LOD_INRANGE)
    {
      (*lit)->removeDirtyFlag(GroupNode::LOD_INRANGE);

      removeTreeFromCaches((*lit)->getFirstChild(), (NodeTypes)(LODNODE | GEONODE | BILLBOARDNODE | PARTICLENODE | LIGHTNODE), Traverser::TRAVERSE_DEFAULT);
    }
  }
}