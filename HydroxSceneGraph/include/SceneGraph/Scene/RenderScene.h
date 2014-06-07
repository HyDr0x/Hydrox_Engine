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

      RenderScene(GroupNode* rootNode, util::EventManager& eventManger, const util::Vector<float, 3>& cameraPosition);
      ~RenderScene();

      GroupNode* addParentNode(unsigned int eventID, TreeNode* destinationNode, const GroupNode* sourceNode);
      TreeNode* addChildNode(unsigned int eventID, GroupNode* destinationNode, const TreeNode* sourceNode);
      void removeNode(TreeNode* node);

      GroupNode* addSubTree(Scene& subScene, GroupNode* sceneNode, const util::Vector<float, 3>& cameraPosition, std::string namePrefix = std::string(""));
      void removeSubTree(TreeNode* sceneNode);

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
