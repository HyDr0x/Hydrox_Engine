#include "Hydrox/Utility/Traverser/CullTraverser.h"

#include "Hydrox/Utility/Tree/LODNode.h"


CullTraverser::CullTraverser(std::vector<float> lodRanges, Vector<float, 3> camPos) : m_lodRanges(lodRanges), m_camPos(camPos)
{
}

CullTraverser::~CullTraverser()
{
}

bool CullTraverser::preTraverse(AnimatedTransformNode* treeNode)
{
  m_dirtyAnimatedTransforms.push_back(treeNode);

  return true;
}

void CullTraverser::postTraverse(AnimatedTransformNode* treeNode)
{
}

bool CullTraverser::preTraverse(TransformNode* treeNode)
{
  m_dirtyTransforms.push_back(treeNode);

  return true;
}

void CullTraverser::postTraverse(TransformNode* treeNode)
{
}

bool CullTraverser::preTraverse(LODNode* treeNode)
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

void CullTraverser::postTraverse(LODNode* treeNode)
{
}

bool CullTraverser::preTraverse(AnimatedGeoNode* treeNode)
{
  m_activeAnimatedGeometry.push_back(treeNode);

  return true;
}

void CullTraverser::postTraverse(AnimatedGeoNode* treeNode)
{
}


bool CullTraverser::preTraverse(GeoNode* treeNode)
{
  m_activeGeometry.push_back(treeNode);

  return true;
}

void CullTraverser::postTraverse(GeoNode* treeNode)
{
}

bool CullTraverser::preTraverse(BillboardNode* treeNode)
{
  m_activeBillboards.push_back(treeNode);

  return true;
}

void CullTraverser::postTraverse(BillboardNode* treeNode)
{
}

bool CullTraverser::preTraverse(ParticleNode* treeNode)
{
  m_activeParticles.push_back(treeNode);

  return true;
}

void CullTraverser::postTraverse(ParticleNode* treeNode)
{
}

bool CullTraverser::preTraverse(LightNode* treeNode)
{
  m_activeLights.push_back(treeNode);

  return true;
}

void CullTraverser::postTraverse(LightNode* treeNode)
{
}

std::list<AnimatedTransformNode*>& CullTraverser::getActiveAnimatedTransformNodes()
{
  return m_dirtyAnimatedTransforms;
}

std::list<TransformNode*>& CullTraverser::getActiveTransformNodes()
{
  return m_dirtyTransforms;
}

std::list<LODNode*>& CullTraverser::getActiveLODNodes()
{
  return m_activeLod;
}

std::list<AnimatedGeoNode*>& CullTraverser::getActiveAnimatedGeoNodes()
{
  return m_activeAnimatedGeometry;
}

std::list<GeoNode*>& CullTraverser::getActiveGeoNodes()
{
  return m_activeGeometry;
}

std::list<BillboardNode*>& CullTraverser::getActiveBillboardNodes()
{
  return m_activeBillboards;
}

std::list<ParticleNode*>& CullTraverser::getActiveParticleNodes()
{
  return m_activeParticles;
}

std::list<LightNode*>& CullTraverser::getActiveLightNodes()
{
  return m_activeLights;
}

