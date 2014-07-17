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

      RenderScene(util::EventManager& eventManger, const util::Vector<float, 3>& cameraPosition, const TreeNodeAllocator& allocator, NodeIndex rootNode);
      ~RenderScene();

      NodeIndex addParentNode(NodeIndex destinationNode, const NodeIndex sourceNode);
      NodeIndex addChildNode(NodeIndex destinationNode, const NodeIndex sourceNode);
      void removeNode(NodeIndex node);

      NodeIndex addSubTree(Scene& subScene, NodeIndex sceneNode, const util::Vector<float, 3>& cameraPosition, std::string namePrefix = std::string(""));
      void removeSubTree(NodeIndex sceneNode);

      void setLODRanges(const std::vector<float>& lodRanges);
      void updateScene(const util::Vector<float, 3>& cameraPosition, float currentTime, bool isTimeRelative = true);

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
