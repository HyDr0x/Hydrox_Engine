#include "SceneGraph/Scene/RenderScene.h"

#include <cassert>
#include <stack>

#include "SceneGraph/Scene/SceneCacheManager.h"

#include "SceneGraph/Traverser/TransformTraverser.h"
#include "SceneGraph/Traverser/DeleteTraverser.h"
#include "SceneGraph/Traverser/RemoveNodesTraverser.h"

#include "SceneGraph/TreeNodes/TransformNode.h"

namespace he
{
  namespace sg
  {
    RenderScene::RenderScene(util::SharedPointer<util::EventManager> eventManger, const util::vec3f& cameraPosition, const TreeNodeAllocator& allocator, NodeIndex rootNode) :
      Scene(allocator, rootNode), m_eventManager(eventManger), m_sceneCacheManager(eventManger)
    {
      TransformTraverser transformTraverser(m_allocator);
      transformTraverser.doTraverse(getNode<TreeNode>(m_rootNode));

      m_sceneCacheManager.addSubTree(m_allocator, getNode<TreeNode>(m_rootNode), cameraPosition);
    }

    RenderScene::~RenderScene()
    {
      m_sceneCacheManager.removeSubTree(m_allocator, getNode<TreeNode>(m_rootNode));
    }

    NodeIndex RenderScene::addParentNode(NodeIndex destinationNode, const TreeNode& sourceNode)
    {
      NodeIndex newNodeIndex = Scene::addParentNode(destinationNode, sourceNode);

      TreeNode& newNode = getNode<TreeNode>(newNodeIndex);

      TransformTraverser transformTraverser(m_allocator);
      transformTraverser.doAscend(newNode);
      transformTraverser.doTraverse(newNode);

      m_sceneCacheManager.addSubTree(m_allocator, newNode, util::vec3f::identity());

      return newNodeIndex;
    }

    NodeIndex RenderScene::addChildNode(NodeIndex destinationNode, const TreeNode& sourceNode)
    {
      NodeIndex newNodeIndex = Scene::addChildNode(destinationNode, sourceNode);

      TreeNode& newNode = getNode<TreeNode>(newNodeIndex);

      TransformTraverser transformTraverser(m_allocator);
      transformTraverser.doAscend(newNode);
      transformTraverser.doTraverse(newNode);

      m_sceneCacheManager.addSubTree(m_allocator, newNode, util::vec3f::identity());

      return newNodeIndex;
    }

    void RenderScene::removeNode(NodeIndex node)
    {
      m_sceneCacheManager.removeSubTree(m_allocator, getNode<TreeNode>(node));

      Scene::removeNode(node);
    }

    NodeIndex RenderScene::addSubTree(const Scene& subTree, NodeIndex parentNodeIndex, const util::vec3f& cameraPosition, std::string namePrefix)
    {
      NodeIndex copiedRootNodeIndex = Scene::addSubTree(subTree, parentNodeIndex, cameraPosition, namePrefix);

      TreeNode& copiedRootNode = getNode<TreeNode>(copiedRootNodeIndex);

      TransformTraverser transformTraverser(m_allocator);
      transformTraverser.doAscend(copiedRootNode);
      transformTraverser.doTraverse(copiedRootNode);

      m_sceneCacheManager.addSubTree(m_allocator, copiedRootNode, cameraPosition);

      return copiedRootNodeIndex;
    }

    void RenderScene::removeSubTree(NodeIndex sceneNode)
    {
      m_sceneCacheManager.removeSubTree(m_allocator, getNode<TreeNode>(sceneNode));

      Scene::removeSubTree(sceneNode);
    }

    void RenderScene::setLODRanges(const std::vector<float>& lodRanges)
    {
      m_sceneCacheManager.setLODRanges(lodRanges);
    }

    void RenderScene::updateScene(const util::vec3f& cameraPosition, float currentTime, bool isTimeRelative)
    {
      m_sceneCacheManager.updateCaches(m_allocator, cameraPosition, currentTime, isTimeRelative);
    }
  }
}