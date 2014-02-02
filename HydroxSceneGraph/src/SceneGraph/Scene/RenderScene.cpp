#include "SceneGraph/Scene/RenderScene.h"

#include <stack>

#include "SceneGraph/Scene/SceneCacheManager.h"

#include "SceneGraph/Traverser/TransformTraverser.h"
#include "SceneGraph/Traverser/InsertObserverTraverser.h"
#include "SceneGraph/Traverser/DeleteTraverser.h"

namespace he
{
	namespace sg
	{
    RenderScene::RenderScene(GroupNode* rootNode, util::EventManager& eventManger, util::Vector<float, 3>& cameraPosition) : Scene(rootNode)
    {
      m_sceneCacheManager = new SceneCacheManager(eventManger);

      TransformTraverser transformTraverser;
      transformTraverser.doTraverse(m_rootNode);

      InsertObserverTraverser insertObserverTraverser(m_sceneCacheManager);
      insertObserverTraverser.doTraverse(m_rootNode);//insert this scene as an observer to every Transform node

      m_sceneCacheManager->addTreeToCaches(m_rootNode, cameraPosition);
    }

    RenderScene::~RenderScene()
    {
      delete m_sceneCacheManager;
    }

    GroupNode* RenderScene::addParentNode(unsigned int eventID, TreeNode *destinationNode, GroupNode *sourceNode)
    {
      GroupNode *newNode = Scene::addParentNode(destinationNode, sourceNode);

      TransformTraverser transformTraverser;
      transformTraverser.doAscend(newNode);
      transformTraverser.doTraverse(newNode);

      m_sceneCacheManager->editNodeCaches(eventID, newNode->getNodeType());

      return newNode;
    }

    TreeNode* RenderScene::addChildNode(unsigned int eventID, GroupNode* destinationNode, TreeNode* sourceNode)
    {
      TreeNode *newNode = Scene::addChildNode(destinationNode, sourceNode);

      TransformTraverser transformTraverser;
      transformTraverser.doAscend(newNode);
      transformTraverser.doTraverse(newNode);

      m_sceneCacheManager->editNodeCaches(eventID, newNode->getNodeType());

      return newNode;
    }

    void RenderScene::removeNode(TreeNode *node)
    {
      m_sceneCacheManager->editNodeCaches(util::EventManager::OnRemoveAnimatedGeometryNode, node->getNodeType());

      Scene::removeNode(node);
    }

    GroupNode* RenderScene::addSubTree(Scene* subScene, GroupNode* sceneNode, util::Vector<float, 3>& cameraPosition, std::string namePrefix)
    {
      GroupNode* copiedRootNode = Scene::addSubTree(subScene, sceneNode, namePrefix);

      TransformTraverser transformTraverser;
      transformTraverser.doAscend(copiedRootNode);
      transformTraverser.doTraverse(copiedRootNode);

      InsertObserverTraverser insertObserverTraverser(m_sceneCacheManager);
      insertObserverTraverser.doTraverse(copiedRootNode);//insert this scene as an observer to every Transform node

      m_sceneCacheManager->addTreeToCaches(copiedRootNode, cameraPosition);

      return copiedRootNode;
    }

    void RenderScene::removeSubTree(TreeNode* sceneNode)
    {
      m_sceneCacheManager->removeTreeFromCaches(sceneNode);

      Scene::removeSubTree(sceneNode);
    }

    void RenderScene::setLODRanges(std::vector<float> lodRanges)
    {
      m_sceneCacheManager->setLODRanges(lodRanges);
    }

    void RenderScene::updateScene(util::Vector<float, 3>& cameraPosition, float currentTime, bool isTimeRelative)
    {
      m_sceneCacheManager->updateCaches(cameraPosition, currentTime, isTimeRelative);
    }
  }
}