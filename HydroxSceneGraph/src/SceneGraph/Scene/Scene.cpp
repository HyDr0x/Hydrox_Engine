#include "SceneGraph/Scene/Scene.h"

#include <assert.h>

#include "SceneGraph/TreeNodes/GroupNode.h"

#include "SceneGraph/Traverser/DeleteTraverser.h"
#include "SceneGraph/Traverser/CopyTraverser.h"
#include "SceneGraph/Traverser/NodeSearchTraverser.h"
#include "SceneGraph/Traverser/AnimationControlTraverser.h"

#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"

namespace he
{
	namespace sg
	{
    Scene::Scene(GroupNode *rootNode)
    {
      m_rootNode = rootNode;
    }

    Scene::Scene(Scene& object)
    {
      addSubTree(&object, m_rootNode, "");
    }

    Scene::~Scene()
    {
      DeleteTraverser deleteTraverser;
      deleteTraverser.doTraverse(m_rootNode);
    }

    GroupNode* Scene::getRootNode()
    {
      return m_rootNode;
    }

    const GroupNode* const Scene::getRootNode() const
    {
      return m_rootNode;
    }

    TreeNode* Scene::searchNode(const std::string& nodeName)
    {
      NodeSearchTraverser traverser(nodeName);

      traverser.doTraverse(m_rootNode);

      return traverser.getDiscoveredNode();
    }

    GroupNode* Scene::addParentNode(TreeNode *destinationNode, GroupNode *sourceNode)
    {
      assert(destinationNode != nullptr && sourceNode != nullptr);

      GroupNode *oldParent = destinationNode->getParent();
      GroupNode *newParent = sourceNode->clone();

      newParent->setParent(oldParent);
      newParent->setNextSibling(destinationNode->getNextSibling());
      newParent->setFirstChild(destinationNode);
      destinationNode->setParent(newParent);
      destinationNode->setNextSibling(nullptr);

      if(oldParent != nullptr)
      {
        if(oldParent->getFirstChild() == destinationNode)//if the destination node was the first child, just change the parent
        {
          oldParent->setFirstChild(newParent);
        }
        else//loop until we find the preceded sibling of the destination node and change its next sibling to the new node
        {
          TreeNode *firstSibling = oldParent->getFirstChild();
          TreeNode *oldSibling = nullptr;
          while(firstSibling != destinationNode)
          {
            oldSibling = firstSibling;
            firstSibling = firstSibling->getNextSibling();
          }

          if(oldSibling != nullptr)
          {
            oldSibling->setNextSibling(newParent);
          }
        }
      }

      return newParent;
    }

    TreeNode* Scene::addChildNode(GroupNode* destinationNode, TreeNode* sourceNode)
    {
      assert(destinationNode != nullptr && sourceNode != nullptr);

      TreeNode* oldFirstChild = destinationNode->getFirstChild();
      TreeNode* newFirstChild = sourceNode->clone();

      destinationNode->setFirstChild(newFirstChild);
      newFirstChild->setNextSibling(oldFirstChild);
      newFirstChild->setParent(destinationNode);

      return newFirstChild;
    }

    void Scene::removeNode(TreeNode *node)
    {
      assert(node != nullptr);

      TreeNode *sibling = node->getNextSibling();
      GroupNode *parent = node->getParent();
      TreeNode *firstChild = node->getFirstChild();

      if(parent->getFirstChild() == node)
      {
        if(firstChild != nullptr)
        {
          parent->setFirstChild(firstChild);

          TreeNode *oldChildSibling;
          while(firstChild != nullptr)
          {
            firstChild->setParent(parent);

            oldChildSibling = firstChild;
            firstChild = firstChild->getNextSibling();
          }

          oldChildSibling->setNextSibling(sibling);
        }
        else
        {
          parent->setFirstChild(sibling);
        }
      }
      else
      {
        TreeNode *parentFirstChild = parent->getFirstChild();
        TreeNode *oldParentFirstChild;
        while(parentFirstChild != node)
        {
          oldParentFirstChild = parentFirstChild;
          parentFirstChild = parentFirstChild->getNextSibling();
        }

        if(firstChild != nullptr)
        {
          oldParentFirstChild->setNextSibling(firstChild);

          TreeNode *oldChildSibling;
          while(firstChild != nullptr)
          {
            firstChild->setParent(parent);

            oldChildSibling = firstChild;
            firstChild = firstChild->getNextSibling();
          }

          oldChildSibling->setNextSibling(sibling);
        }
        else
        {
          oldParentFirstChild->setNextSibling(sibling);
        }
      }

      delete node;
      node = nullptr;
    }

    GroupNode* Scene::addSubTree(Scene* subTree, GroupNode* parentNode, std::string namePrefix)
    {
      assert(subTree != nullptr && parentNode != nullptr);

      CopyTraverser traverser(namePrefix);
      traverser.doTraverse(subTree->getRootNode());
      GroupNode *newNode = traverser.getCopiedRootNode();

      if(parentNode != nullptr && m_rootNode != nullptr)
      {
        TreeNode *oldFirstChild = parentNode->getFirstChild();

        parentNode->setFirstChild(newNode);
        newNode->setNextSibling(oldFirstChild);
        newNode->setParent(parentNode);
      }
      else
      {
        m_rootNode = newNode;
      }

      return newNode;
    }

    void Scene::removeSubTree(TreeNode* rootNode)
    {
      assert(rootNode != nullptr);

      GroupNode* parentNode = rootNode->getParent();

      if(parentNode != nullptr)
      {
        TreeNode* currentSibling = parentNode->getFirstChild();
        TreeNode* predSibling = currentSibling;

        //disconnect the subtree from the whole tree
        while(currentSibling != nullptr)
        {
          if(currentSibling == rootNode)
          {
            if(currentSibling == predSibling)
            {
              parentNode->setFirstChild(currentSibling->getNextSibling());
            }
            else
            {
              predSibling->setNextSibling(currentSibling->getNextSibling());
            }

            break;//leave the loop
          }

          predSibling = currentSibling;
          currentSibling = currentSibling->getNextSibling();
        }
      }

      DeleteTraverser traverser;
      traverser.doTraverse(rootNode);
    }

    void Scene::setAnimatedSceneTime(AnimatedTransformNode *node, float animationTime)
    {
      AnimationControlTraverser traverser;
      traverser.setAnimationTime(animationTime);
      traverser.doTraverse(node);
    }

    void Scene::addAnimatedSceneTime(AnimatedTransformNode *node, float animationTime)
    {
      AnimationControlTraverser traverser;
      traverser.addAnimationTime(animationTime);
      traverser.doTraverse(node);
    }

    void Scene::pauseAnimatedScene(AnimatedTransformNode *node, bool pauseAnimation)
    {
      AnimationControlTraverser traverser;
      traverser.setPauseAnimation(pauseAnimation);
      traverser.doTraverse(node);
    }

    void Scene::stopAnimatedScene(AnimatedTransformNode *node)
    {
      AnimationControlTraverser traverser;
      traverser.setStopAnimation();
      traverser.doTraverse(node);
    }
	}
}