#ifndef SCENECACHEMANAGER_H_
#define SCENECACHEMANAGER_H_

#include <list>
#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Observer/Observer.hpp>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
	namespace sg
	{
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

    class SceneCacheManager : public util::Observer<TransformNode*>
    {
    public:

      SceneCacheManager(util::EventManager& eventManager);
      ~SceneCacheManager();

      void setLODRanges(std::vector<float> lodRanges);

      void updateCaches(util::Vector<float, 3>& cameraPosition, float currentTime, bool isTimeRelative = true);

      template<class NODETYPE> void editNodeCaches(unsigned int eventID, NODETYPE *node)
      {
        m_eventManager.raiseSignal<void (*)(NODETYPE *newNode)>(eventID)->execute(node);
      }

      void addTreeToCaches(TreeNode *rootNode, util::Vector<float, 3>& cameraPosition);
      void removeTreeFromCaches(TreeNode *rootNode);

    private:

      template<class CLASS> void deleteFromCacheList(std::list<CLASS*>& sourceCacheList, std::list<CLASS*>& destinationCacheList)
      {
        for(std::list<CLASS*>::iterator sourceIterator = sourceCacheList.begin(); sourceIterator != sourceCacheList.end(); sourceIterator++)
        {
          destinationCacheList.remove(*sourceIterator);
        }
      }

      SceneCacheManager(const SceneCacheManager&) : m_eventManager(util::EventManager()){}

      void updateObserver(TransformNode* data);
      void updateAnimationTime(float currentTime, bool isTimeRelative);
      void updateAnimatedTransformNodes();
      void updateTransformNodes();
      void updateLODNodes(util::Vector<float, 3>& cameraPosition);

      util::EventManager& m_eventManager;

      std::vector<float> m_lodRanges;

      std::list<TransformNode*> m_dirtyTransforms;
      std::list<LODNode*> m_activeLODs;
      std::list<AnimatedTransformNode*> m_activeAnimatedTransforms;
    };
  }
}

#endif
