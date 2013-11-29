#ifndef SCENECACHEMANAGER
#define SCENECACHEMANAGER

#include <list>
#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"
#include "Hydrox/Utility/Observer/Observer.hpp"
#include "Hydrox/Utility/Traverser/Traverser.h"

class TreeNode;
class GroupNode;
class LODNode;
class AnimatedTransformNode;
class TransformNode;
class AnimatedGeoNode;
class GeoNode;
class BillboardNode;
class ParticleNode;
class LightNode;

class GRAPHICAPI SceneCacheManager : public Observer<TransformNode*>
{
public:

  SceneCacheManager();
  ~SceneCacheManager();

  const std::list<AnimatedGeoNode*>& getAnimatedMeshes();
  const std::list<GeoNode*>& getMeshes();
  const std::list<BillboardNode*>& getBillboards();
  const std::list<LightNode*>& getLights();

  void setLODRanges(std::vector<float> lodRanges);

  void updateCaches(Vector<float, 3>& cameraPosition, float currentTime);

protected:

  void addNodeToCaches(TreeNode *newNode);
  void removeNodeFromCaches(TreeNode *node);
  void addTreeToCaches(TreeNode *rootNode, Vector<float, 3>& cameraPosition);
  void removeTreeFromCaches(TreeNode *rootNode);

private:

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

  SceneCacheManager(const SceneCacheManager&){}

  void updateObserver(TransformNode* data);
  void updateAnimatedTransformNodes(float currentTime);
  void updateTransformNodes();
  void updateLODNodes(Vector<float, 3>& cameraPosition);

  std::vector<float> m_lodRanges;

  std::list<TransformNode*> m_dirtyTransforms;
  std::list<LODNode*> m_activeLODs;
  std::list<AnimatedTransformNode*> m_activeAnimatedTransforms;
  
  std::list<AnimatedGeoNode*> m_activeAnimatedGeometry;
  std::list<GeoNode*> m_activeGeometry;
  std::list<BillboardNode*> m_activeBillboards;
  std::list<ParticleNode*> m_activeParticles;
  std::list<LightNode*> m_activeLights;
};

#endif