#include "Hydrox/Graphics/Scene.h"

#include <stack>

#include "Hydrox/Utility/Miscellaneous/SceneCacheManager.h"

#include "Hydrox/Utility/Traverser/TransformTraverser.h"
#include "Hydrox/Utility/Traverser/CullTraverser.h"
#include "Hydrox/Utility/Traverser/InsertObserverTraverser.h"
#include "Hydrox/Utility/Traverser/DeleteTraverser.h"
#include "Hydrox/Utility/Traverser/NodeSearchTraverser.h"

#include "Hydrox/Utility/Tree/TreeNode.h"
#include "Hydrox/Utility/Tree/GroupNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

Scene::Scene(GroupNode* rootNode, Vector<float, 3>& cameraPosition)
{
  m_rootNode = rootNode;

  TransformTraverser transformTraverser;
  transformTraverser.doTraverse(m_rootNode);

  InsertObserverTraverser insertObserverTraverser(this);
  insertObserverTraverser.doTraverse(m_rootNode);//insert this scene as an observer to every Transform node

  SceneCacheManager::addTreeToCaches(m_rootNode, cameraPosition);
}

Scene::Scene(const Scene&)
{
}

Scene::~Scene()
{
  DeleteTraverser deleteTraverser;
  deleteTraverser.doTraverse(m_rootNode);
}

TreeNode* Scene::addParentNode(TreeNode *destinationNode, GroupNode *sourceNode)
{
  TreeNode *newNode = Tree::addParentNode(destinationNode, sourceNode);

  TransformTraverser transformTraverser;
  transformTraverser.doAscend(newNode);
  transformTraverser.doTraverse(newNode);

  TransformNode *newTransform = dynamic_cast<TransformNode*>(newNode);
  if(newTransform != nullptr)
  {
    newTransform->add(dynamic_cast<Observer<TransformNode*>*>(this));//insert this scene as an observer to every Transform node
  }

  SceneCacheManager::addNodeToCaches(newNode);
  return newNode;
}

TreeNode* Scene::addChildNode(GroupNode* destinationNode, TreeNode* sourceNode)
{
  TreeNode *newNode = Tree::addChildNode(destinationNode, sourceNode);

  TransformTraverser transformTraverser;
  transformTraverser.doAscend(newNode);
  transformTraverser.doTraverse(newNode);

  TransformNode *newTransform = dynamic_cast<TransformNode*>(newNode);
  if(newTransform != nullptr)
  {
    newTransform->add(dynamic_cast<Observer<TransformNode*>*>(this));//insert this scene as an observer to every Transform node
  }

  SceneCacheManager::addNodeToCaches(newNode);
    return newNode;
}

void Scene::removeNode(TreeNode *node)
{
  SceneCacheManager::removeNodeFromCaches(node);

  Tree::removeNode(node);
}

GroupNode* Scene::addSubTree(Tree* object, GroupNode* sceneNode, Vector<float, 3>& cameraPosition, std::string namePrefix)
{
  GroupNode* copiedRootNode = Tree::addSubTree(object, sceneNode, namePrefix);

  TransformTraverser transformTraverser;
  transformTraverser.doAscend(copiedRootNode);
  transformTraverser.doTraverse(copiedRootNode);

  InsertObserverTraverser insertObserverTraverser(this);
  insertObserverTraverser.doTraverse(copiedRootNode);//insert this scene as an observer to every Transform node

  SceneCacheManager::addTreeToCaches(copiedRootNode, cameraPosition);

  return copiedRootNode;
}

void Scene::removeSubTree(TreeNode* sceneNode)
{
  SceneCacheManager::removeTreeFromCaches(sceneNode);

  Tree::removeSubTree(sceneNode);
}

TreeNode* Scene::searchNode(const std::string& nodeName)
{
  NodeSearchTraverser traverser(nodeName);

  traverser.doTraverse(m_rootNode);

  return traverser.getDiscoveredNode();
}