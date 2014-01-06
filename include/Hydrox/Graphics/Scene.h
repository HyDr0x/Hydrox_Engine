#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#include <list>
#include <string>
#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"
#include "Hydrox/Utility/Tree/Tree.h"

#include "Hydrox/Graphics/SceneCacheManager.h"

namespace he
{
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

	  Scene(GroupNode* rootNode, Vector<float, 3>& cameraPosition);
    Scene(const Scene& object);
	  ~Scene();

    GroupNode* addParentNode(TreeNode *destinationNode, GroupNode *sourceNode);
    TreeNode* addChildNode(GroupNode *destinationNode, TreeNode *sourceNode);
    void removeNode(TreeNode *node);

	  GroupNode* addSubTree(Tree* object, GroupNode* sceneNode, Vector<float, 3>& cameraPosition, std::string namePrefix = std::string(""));
    void removeSubTree(TreeNode* sceneNode);

    template<class CLASS> CLASS* getNode(const std::string& nodeName)
    {
      return dynamic_cast<CLASS*>(searchNode(nodeName));
    }

  protected:

	  Scene(){}
	  Scene& operator=(const Scene&){ return *this; }

  private:

    TreeNode* searchNode(const std::string& nodeName);

  };
}

#endif
