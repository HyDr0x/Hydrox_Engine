#ifndef SCENE_H_
#define SCENE_H_

#include <map>
#include <string>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>

#include "SceneGraph/Scene/TreeNodeAllocator.h"

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

      Scene(const TreeNodeAllocator& m_allocator, NodeIndex rootNode);
      Scene(const Scene& object);
      virtual ~Scene();

      Scene& operator=(Scene& other);

      NodeIndex getRootNode();
      const NodeIndex const getRootNode() const;

      NodeIndex searchNode(const std::string& nodeName);

      NodeIndex searchNode(const std::string& nodeName, NodeIndex searchRootNode);

      virtual NodeIndex addParentNode(NodeIndex destinationNodeIndex, const TreeNode& sourceNode);
      virtual NodeIndex addChildNode(NodeIndex destinationNodeIndex, const TreeNode& sourceNode);
      virtual void removeNode(const NodeIndex node);

      virtual NodeIndex addSubTree(const Scene& subTree, NodeIndex parentNodeIndex, const util::Vector<float, 3>& cameraPosition, std::string namePrefix);
      virtual void removeSubTree(const NodeIndex rootNodeIndex);

      template<typename T> T& getNode(const std::string& nodeName)
      {
        return dynamic_cast<T&>(m_allocator[searchNode(nodeName)]);
      }

      template<typename T> T& getNode(const std::string& nodeName, const NodeIndex searchRootNode)
      {
        return dynamic_cast<T&>(m_allocator[searchNode(nodeName, searchRootNode)]);
      }

      template<typename T> T& getNode(NodeIndex index)
      {
        return dynamic_cast<T&>(m_allocator[index]);
      }

      template<typename T> const T& getNode(NodeIndex index) const
      {
        return dynamic_cast<T&>(m_allocator[index]);
      }

      TreeNodeAllocator& getTreeNodeAllocator();
      const TreeNodeAllocator& getTreeNodeAllocator() const;

      void setAnimatedSceneTime(AnimatedTransformNode& node, float animationTime);
      void addAnimatedSceneTime(AnimatedTransformNode& node, float animationTime);
      void pauseAnimatedScene(AnimatedTransformNode& node, bool pauseAnimation);
      void stopAnimatedScene(AnimatedTransformNode& node);

    protected:

      NodeIndex m_rootNode;//the root node

      TreeNodeAllocator m_allocator;
    };
  }
}

#endif
