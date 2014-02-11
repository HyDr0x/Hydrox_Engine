#ifndef RENDERSCENE_H_
#define RENDERSCENE_H_

#include <list>
#include <string>
#include <vector>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
//#include <Utilities/Observer/Observer.hpp>
#include <Utilities/Signals/EventManager.h>

#include "SceneGraph/Scene/Scene.h"

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

    class GRAPHICAPI RenderScene : public Scene/*, public util::Observer<TransformNode*>*/
    {
    public:

	    RenderScene(GroupNode* rootNode, util::EventManager& eventManger, util::Vector<float, 3>& cameraPosition);
	    ~RenderScene();

      GroupNode* addParentNode(unsigned int eventID, TreeNode *destinationNode, GroupNode *sourceNode);
      TreeNode* addChildNode(unsigned int eventID, GroupNode *destinationNode, TreeNode *sourceNode);
      void removeNode(TreeNode *node);

	    GroupNode* addSubTree(Scene* subScene, GroupNode* sceneNode, util::Vector<float, 3>& cameraPosition, std::string namePrefix = std::string(""));
      void removeSubTree(TreeNode* sceneNode);

      void setLODRanges(std::vector<float> lodRanges);
      void updateScene(util::Vector<float, 3>& cameraPosition, float currentTime, bool isTimeRelative = true);

    private:
 
	    RenderScene() : Scene(new GroupNode("")), m_eventManager(util::EventManager()) {}
      RenderScene(const RenderScene& object) : Scene(object.m_rootNode), m_eventManager(object.m_eventManager) {}
	    RenderScene& operator=(const RenderScene&){ return *this; }

      //void updateObserver(TransformNode* data);

      SceneCacheManager *m_sceneCacheManager;

      util::EventManager& m_eventManager;

      //std::vector<float> m_lodRanges;
    };
  }
}

#endif
