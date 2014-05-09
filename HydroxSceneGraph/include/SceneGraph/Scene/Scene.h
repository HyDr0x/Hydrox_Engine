#ifndef SCENE_H_
#define SCENE_H_

#include <string>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>

namespace he
{
	namespace sg
	{
    class TreeNode;
    class GroupNode;
    class AnimatedTransformNode;

    class GRAPHICAPI Scene
    {
    public:

      Scene(GroupNode* rootNode);
      Scene(Scene& object);
      virtual ~Scene();

      Scene& operator=(Scene& other);

      GroupNode* getRootNode();
      const GroupNode* const getRootNode() const;

      TreeNode* searchNode(const std::string& nodeName) const;

      TreeNode* searchNode(const std::string& nodeName, TreeNode *searchRootNode) const;

      virtual GroupNode* addParentNode(TreeNode *destinationNode, const GroupNode *sourceNode);
      virtual TreeNode* addChildNode(GroupNode *destinationNode, const TreeNode *sourceNode);
      virtual void removeNode(const TreeNode *node);

      virtual GroupNode* addSubTree(Scene& subTree, GroupNode *parentNode, const util::Vector<float, 3>& cameraPosition, std::string namePrefix);
      virtual void removeSubTree(TreeNode *rootNode);

      template<class CLASS> CLASS* getNode(const std::string& nodeName) const
      {
        return dynamic_cast<CLASS*>(searchNode(nodeName));
      }

      template<class CLASS> CLASS* getNode(const std::string& nodeName, const TreeNode *searchRootNode) const
      {
        return dynamic_cast<CLASS*>(searchNode(nodeName, searchRootNode));
      }

      static void setAnimatedSceneTime(AnimatedTransformNode* node, float animationTime);
      static void addAnimatedSceneTime(AnimatedTransformNode* node, float animationTime);
      static void pauseAnimatedScene(AnimatedTransformNode* node, bool pauseAnimation);
      static void stopAnimatedScene(AnimatedTransformNode* node);

    protected:

      GroupNode *m_rootNode;//the root node
    };
	}
}

#endif
