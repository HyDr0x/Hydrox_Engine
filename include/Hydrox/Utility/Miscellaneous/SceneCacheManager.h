#ifndef SCENECACHEMANAGER
#define SCENECACHEMANAGER

#include <list>
#include <vector>

#include "Hydrox/Utility/Observer/Observer.hpp"

#include "Hydrox/Utility/Traverser/Traverser.h"

class Camera;

class TreeNode;
class GroupNode;
class LODNode;
class TransformNode;
class GeoNode;
class BillboardNode;
class ParticleNode;
class LightNode;

class GRAPHICAPI SceneCacheManager : public Observer<TransformNode*>
{
public:

  enum NodeTypes
  {
    TRANSFORMNODE = 1,
    LODNODE       = 2,
    GEONODE       = 4,
    BILLBOARDNODE = 8,
    LIGHTNODE     = 16,
    PARTICLENODE  = 32,
  };

  SceneCacheManager();
  SceneCacheManager(Camera *camera);
  ~SceneCacheManager();

  const std::list<GeoNode*>& getMeshes();
  const std::list<BillboardNode*>& getBillboards();
  const std::list<LightNode*>& getLights();

  void setLODRanges(std::vector<float> lodRanges);

  void updateCaches();

protected:

  void addNodeToCaches(TreeNode *newNode, NodeTypes types);
  void removeNodeFromCaches(TreeNode *node, NodeTypes types);

  void addTreeToCaches(TreeNode *rootNode, NodeTypes types, Traverser::TraverserFlags traverserFlag);
  void removeTreeFromCaches(TreeNode *rootNode, NodeTypes types, Traverser::TraverserFlags traverserFlag);

private:

  SceneCacheManager(const SceneCacheManager&){}

  template<class CLASS> bool addSingleNodeToCacheList(std::list<CLASS*>& destinationCacheList, TreeNode* newNode)
  {
    CLASS *specializedNode = dynamic_cast<CLASS*>(newNode);

    if(specializedNode != nullptr)
    {
      destinationCacheList.push_back(specializedNode);
      return true;
    }

    return false;
  }

  template<class CLASS> bool removeSingleNodeFromCacheList(std::list<CLASS*>& sourceCacheList, TreeNode* oldNode)
  {
    CLASS *specializedNode = dynamic_cast<CLASS*>(oldNode);

    if(specializedNode != nullptr)
    {
      sourceCacheList.remove(specializedNode);
      return true;
    }

    return false;
  }

  template<class CLASS> void addToCacheList(std::list<CLASS*>& sourceCacheList, std::list<CLASS*>& destinationCacheList)
  {
    for(std::list<CLASS*>::iterator insertIterator = sourceCacheList.begin(); insertIterator != sourceCacheList.end(); insertIterator++)
    {
      destinationCacheList.push_back(*insertIterator);
    }
  }

  template<class CLASS> void deleteFromCacheList(std::list<CLASS*>& sourceCacheList, std::list<CLASS*>& destinationCacheList)
  {
    for(std::list<CLASS*>::iterator sourceIterator = sourceCacheList.begin(); sourceIterator != sourceCacheList.end(); sourceIterator++)
    {
      for(std::list<CLASS*>::iterator destinationIterator = destinationCacheList.begin(); destinationIterator != destinationCacheList.end(); destinationIterator++)
      {
        if(*sourceIterator == *destinationIterator)
        {
          destinationCacheList.erase(destinationIterator);
          break;
        }
      }
    }
  }

  void updateObserver(TransformNode* data);

  void updateTransformNodes();
  void updateLODNodes();

  Camera *m_camera;

  std::vector<float> m_lodRanges;

  std::list<TransformNode*> m_dirtyTransforms;

  std::list<GeoNode*> m_activeGeometry;
  std::list<LODNode*> m_activeLODs;
  std::list<BillboardNode*> m_activeBillboards;
  std::list<ParticleNode*> m_activeParticles;
  std::list<LightNode*> m_activeLights;
};

#endif