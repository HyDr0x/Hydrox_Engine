#include "Hydrox/Graphics/Scene.h"

#include <stack>

#include "Hydrox/Utility/Miscellaneous/SceneCacheManager.h"

#include "Hydrox/Utility/Traverser/TransformTraverser.h"
#include "Hydrox/Utility/Traverser/CullTraverser.h"
#include "Hydrox/Utility/Traverser/InsertObserverTraverser.h"
#include "Hydrox/Utility/Traverser/DeleteTraverser.h"

#include "Hydrox/Services/Camera.h"

#include "Hydrox/Utility/Tree/TreeNode.h"
#include "Hydrox/Utility/Tree/GroupNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

Scene::Scene(GroupNode* rootNode, Camera *camera) : SceneCacheManager(camera)
{
  m_rootNode = rootNode;

  TransformTraverser transformTraverser;
  transformTraverser.doTraverse(m_rootNode);

  InsertObserverTraverser insertObserverTraverser(this);
  insertObserverTraverser.doTraverse(m_rootNode);//insert this scene as an observer to every Transform node

  SceneCacheManager::addTreeToCaches(m_rootNode, (SceneCacheManager::NodeTypes)(SceneCacheManager::LODNODE | 
                                                                                SceneCacheManager::GEONODE | 
                                                                                SceneCacheManager::BILLBOARDNODE | 
                                                                                SceneCacheManager::PARTICLENODE | 
                                                                                SceneCacheManager::LIGHTNODE), Traverser::TRAVERSE_DEFAULT);
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

  TransformTraverser transformUpTraversal;
  transformUpTraversal.doAscend(newNode);

  std::stack<Mat<float, 4>> matrixStack = transformUpTraversal.getMatrixStack();
  Mat<float, 4> finalTrfMatrix = Mat<float, 4>::identity();

  while(!matrixStack.empty())
  {
    finalTrfMatrix *= matrixStack.top();
    matrixStack.pop();
  }

  TransformTraverser transformTraverser;
  transformTraverser.insertTransformMatrix(finalTrfMatrix);
  transformTraverser.doTraverse(newNode);

  TransformNode *newTransform = dynamic_cast<TransformNode*>(newNode);
  if(newTransform != nullptr)
  {
    newTransform->add(dynamic_cast<Observer<TransformNode*>*>(this));//insert this scene as an observer to every Transform node
  }

  SceneCacheManager::addNodeToCaches(newNode, (SceneCacheManager::NodeTypes)(SceneCacheManager::LODNODE | 
                                                                             SceneCacheManager::GEONODE | 
                                                                             SceneCacheManager::BILLBOARDNODE | 
                                                                             SceneCacheManager::PARTICLENODE | 
                                                                             SceneCacheManager::LIGHTNODE));
  return newNode;
}

TreeNode* Scene::addChildNode(GroupNode* destinationNode, TreeNode* sourceNode)
{
  TreeNode *newNode = Tree::addChildNode(destinationNode, sourceNode);

  TransformTraverser transformUpTraversal;
  transformUpTraversal.doAscend(newNode);

  std::stack<Mat<float, 4>> matrixStack = transformUpTraversal.getMatrixStack();
  Mat<float, 4> finalTrfMatrix = Mat<float, 4>::identity();

  while(!matrixStack.empty())
  {
    finalTrfMatrix *= matrixStack.top();
    matrixStack.pop();
  }

  TransformTraverser transformTraverser;
  transformTraverser.insertTransformMatrix(finalTrfMatrix);
  transformTraverser.doTraverse(newNode);

  TransformNode *newTransform = dynamic_cast<TransformNode*>(newNode);
  if(newTransform != nullptr)
  {
    newTransform->add(dynamic_cast<Observer<TransformNode*>*>(this));//insert this scene as an observer to every Transform node
  }

  SceneCacheManager::addNodeToCaches(newNode, (SceneCacheManager::NodeTypes)(SceneCacheManager::LODNODE | 
                                                                             SceneCacheManager::GEONODE | 
                                                                             SceneCacheManager::BILLBOARDNODE | 
                                                                             SceneCacheManager::PARTICLENODE | 
                                                                             SceneCacheManager::LIGHTNODE));
    return newNode;
}

void Scene::removeNode(TreeNode *node)
{

  SceneCacheManager::removeNodeFromCaches(node, (SceneCacheManager::NodeTypes)(SceneCacheManager::TRANSFORMNODE |
                                                                               SceneCacheManager::LODNODE | 
                                                                               SceneCacheManager::GEONODE | 
                                                                               SceneCacheManager::BILLBOARDNODE | 
                                                                               SceneCacheManager::PARTICLENODE | 
                                                                               SceneCacheManager::LIGHTNODE));

  Tree::removeNode(node);
}

GroupNode* Scene::addSubTree(Tree* object, GroupNode* sceneNode, std::string namePrefix)
{
  GroupNode* copiedRootNode = Tree::addSubTree(object, sceneNode, namePrefix);

  TransformTraverser transformUpTraversal;
  transformUpTraversal.doAscend(copiedRootNode);

  std::stack<Mat<float, 4>> matrixStack = transformUpTraversal.getMatrixStack();
  Mat<float, 4> finalTrfMatrix = Mat<float, 4>::identity();

  while(!matrixStack.empty())
  {
    finalTrfMatrix *= matrixStack.top();
    matrixStack.pop();
  }

  TransformTraverser transformTraverser;
  transformTraverser.insertTransformMatrix(finalTrfMatrix);
  transformTraverser.doTraverse(copiedRootNode);

  InsertObserverTraverser insertObserverTraverser(this);
  insertObserverTraverser.doTraverse(copiedRootNode);//insert this scene as an observer to every Transform node

  SceneCacheManager::addTreeToCaches(copiedRootNode, (SceneCacheManager::NodeTypes)(SceneCacheManager::LODNODE | 
                                                                                    SceneCacheManager::GEONODE | 
                                                                                    SceneCacheManager::BILLBOARDNODE | 
                                                                                    SceneCacheManager::PARTICLENODE | 
                                                                                    SceneCacheManager::LIGHTNODE), Traverser::TRAVERSE_DEFAULT);

  return copiedRootNode;
}

void Scene::removeSubTree(TreeNode* sceneNode)
{
  SceneCacheManager::removeTreeFromCaches(sceneNode, (SceneCacheManager::NodeTypes)(SceneCacheManager::TRANSFORMNODE |
                                                                                    SceneCacheManager::LODNODE | 
                                                                                    SceneCacheManager::GEONODE | 
                                                                                    SceneCacheManager::BILLBOARDNODE | 
                                                                                    SceneCacheManager::PARTICLENODE | 
                                                                                    SceneCacheManager::LIGHTNODE), Traverser::TRAVERSE_IGNORE_LODS);

  Tree::removeSubTree(sceneNode);
}