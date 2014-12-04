#ifndef RENDERSCENE_H_
#define RENDERSCENE_H_

#include <list>
#include <string>
#include <vector>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include <Utilities/Signals/EventManager.h>

#include "SceneGraph/Scene/Scene.h"
#include "SceneGraph/Scene/SceneCacheManager.h"

#include "SceneGraph/TreeNodes/GroupNode.h"

namespace he
{
  namespace sg
  {
    class SceneCacheManager;

    class TreeNode;
    class GroupNode;
    class LODNode;
    class TransformNode;
    class GeoNode;
    class BillboardNode;
    class ParticleNode;
    class LightNode;

    class GRAPHICAPI RenderScene : public Scene
    {
    public:

      RenderScene(util::EventManager& eventManger, const util::vec3f& cameraPosition, const TreeNodeAllocator& allocator, NodeIndex rootNode);
      ~RenderScene();

      NodeIndex addParentNode(NodeIndex destinationNode, const TreeNode& sourceNode);
      NodeIndex addChildNode(NodeIndex destinationNode, const TreeNode& sourceNode);
      void removeNode(NodeIndex node);

      NodeIndex addSubTree(const Scene& subTree, NodeIndex parentNodeIndex, const util::vec3f& cameraPosition, std::string namePrefix = std::string(""));
      void removeSubTree(NodeIndex sceneNode);

      void setLODRanges(const std::vector<float>& lodRanges);
      void updateScene(const util::vec3f& cameraPosition, float currentTime, bool isTimeRelative = true);

    private:
 
      RenderScene();
      RenderScene(const RenderScene& object);
      RenderScene& operator=(const RenderScene&);

      SceneCacheManager m_sceneCacheManager;

      util::EventManager& m_eventManager;
    };
  }
}

#endif
