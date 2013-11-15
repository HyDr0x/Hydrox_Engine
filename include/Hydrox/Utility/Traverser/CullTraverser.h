#ifndef CULLTRAVERSER_H_
#define CULLTRAVERSER_H_

#include <list>
#include <vector>

#include "Hydrox/Utility/Traverser/Traverser.h"
#include "Hydrox/Utility/Math/Math.hpp"

class CullTraverser : public Traverser
{
public:

  CullTraverser(std::vector<float> lodRanges, Vector<float, 3> camPos);
  virtual ~CullTraverser();

  virtual bool preTraverse(TransformNode* treeNode);
  virtual void postTraverse(TransformNode* treeNode);

  virtual bool preTraverse(LODNode* treeNode);
  virtual void postTraverse(LODNode* treeNode);

  virtual bool preTraverse(GeoNode* treeNode);
  virtual void postTraverse(GeoNode* treeNode);

  virtual bool preTraverse(BillboardNode* treeNode);
  virtual void postTraverse(BillboardNode* treeNode);

  virtual bool preTraverse(ParticleNode* treeNode);
  virtual void postTraverse(ParticleNode* treeNode);

  virtual bool preTraverse(LightNode* treeNode);
  virtual void postTraverse(LightNode* treeNode);

  std::list<TransformNode*>& getActiveTransformNodes();
  std::list<LODNode*>& getActiveLODNodes();
  std::list<GeoNode*>& getActiveGeoNodes();
  std::list<BillboardNode*>& getActiveBillboardNodes();
  std::list<ParticleNode*>& getActiveParticleNodes();
  std::list<LightNode*>& getActiveLightNodes();

protected:

  Vector<float, 3> m_camPos;
  std::vector<float> m_lodRanges;

  std::list<TransformNode*> m_dirtyTransforms;
  std::list<LODNode*> m_activeLod;
  std::list<GeoNode*> m_activeGeometry;
  std::list<BillboardNode*> m_activeBillboards;
  std::list<ParticleNode*> m_activeParticles;
  std::list<LightNode*> m_activeLights;
};

#endif
