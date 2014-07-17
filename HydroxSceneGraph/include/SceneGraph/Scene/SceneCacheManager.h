#ifndef SCENECACHEMANAGER_H_
#define SCENECACHEMANAGER_H_

#include <list>
#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Observer/Observer.hpp>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "SceneGraph/Traverser/Traverser.h"

#include "SceneGraph/Scene/TreeNodeAllocator.h"

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

      void setLODRanges(const std::vector<float>& lodRanges);

      void addSubTree(TreeNodeAllocator& allocator, TreeNode& rootNode, const util::Vector<float, 3>& cameraPosition);
      void removeSubTree(TreeNodeAllocator& allocator, TreeNode& rootNode);

      void updateCaches(TreeNodeAllocator& allocator, const util::Vector<float, 3>& cameraPosition, float currentTime, bool isTimeRelative = true);

      void addNodeToCache(sg::LODNode *lodNode);
      void addNodeToCache(sg::TransformNode *transformNode);
      void addNodeToCache(sg::AnimatedTransformNode *animatedTransformNode);

      void removeNodeFromCache(sg::LODNode *lodNode);
      void removeNodeFromCache(sg::TransformNode *transformNode);
      void removeNodeFromCache(sg::AnimatedTransformNode *animatedTransformNode);

    private:

      //SceneCacheManager(const SceneCacheManager& object) : m_eventManager(object.m_eventManager){}

      void registerNodeCacheSlots();

      void update(TransformNode *data) override;

      void updateAnimationTime(float currentTime, bool isTimeRelative);
      void updateAnimatedTransformNodes(TreeNodeAllocator& allocator);
      void updateTransformNodes(TreeNodeAllocator& allocator);
      void updateLODNodes(TreeNodeAllocator& allocator, const util::Vector<float, 3>& cameraPosition);

      util::EventManager& m_eventManager;

      std::vector<float> m_lodRanges;

      std::list<TransformNode*> m_dirtyTransforms;
      std::list<AnimatedTransformNode*> m_activeAnimatedTransforms;
      std::list<LODNode*> m_activeLODs;
    };
  }
}

#endif
