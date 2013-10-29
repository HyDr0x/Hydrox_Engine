#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#include <list>
#include <string>
#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Traverser/NodeSearchTraverser.h"

#include "Hydrox/Utility/Math/Math.hpp"
#include "Hydrox/Utility/Tree/Tree.h"

#include "Hydrox/Utility/Miscellaneous/SceneCacheManager.h"

class Camera;
class TreeNode;
class GroupNode;
class LODNode;
class TransformNode;
class GeoNode;
class BillboardNode;
class ParticleNode;
class LightNode;

class GRAPHICAPI Scene : public Tree, public SceneCacheManager
{
public:

	Scene(GroupNode* rootNode, Camera *camera);
  Scene(const Scene& object);
	~Scene();

  TreeNode* addParentNode(TreeNode *destinationNode, GroupNode *sourceNode);
  TreeNode* addChildNode(GroupNode *destinationNode, TreeNode *sourceNode);
  void removeNode(TreeNode *node);

	GroupNode* addSubTree(Tree* object, GroupNode* sceneNode, std::string namePrefix = std::string(""));
  void removeSubTree(TreeNode* sceneNode);

  template<class CLASS> CLASS* getNode(std::string nodeName)
  {
    NodeSearchTraverser traverser(nodeName);

    traverser.doTraverse(m_rootNode);

    return dynamic_cast<CLASS*>(traverser.getDiscoveredNode());
  }

protected:

	Scene(){}
	Scene& operator=(const Scene&){ return *this; }

private:

};

#endif