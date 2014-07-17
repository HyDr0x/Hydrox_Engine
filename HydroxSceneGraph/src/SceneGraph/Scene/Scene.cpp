#include "SceneGraph/Scene/Scene.h"

#include <assert.h>

#include "SceneGraph/TreeNodes/GroupNode.h"

#include "SceneGraph/Traverser/CopyTraverser.h"
#include "SceneGraph/Traverser/DeleteTraverser.h"
#include "SceneGraph/Traverser/NodeSearchTraverser.h"
#include "SceneGraph/Traverser/AnimationControlTraverser.h"

#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"

namespace he
{
  namespace sg
  {
    Scene::Scene(const TreeNodeAllocator& allocator, NodeIndex rootNode) : m_allocator(allocator), m_rootNode(rootNode)
    {
    }

    Scene::Scene(const Scene& object) : m_allocator(object.getTreeNodeAllocator()), m_rootNode(object.getRootNode())
    {
      addSubTree(object, m_rootNode, he::util::Vector<float, 3>::identity(), "");
    }

    Scene::~Scene()
    {
    }

    Scene& Scene::operator=(Scene& other)
    {
      addSubTree(other, m_rootNode, he::util::Vector<float, 3>::identity(), "");

      return *this;
    }

    NodeIndex Scene::getRootNode()
    {
      return m_rootNode;
    }

    const NodeIndex const Scene::getRootNode() const
    {
      return m_rootNode;
    }

    NodeIndex Scene::searchNode(const std::string& nodeName)
    {
      NodeSearchTraverser traverser(m_allocator, nodeName);

      traverser.doTraverse(m_allocator[m_rootNode]);

      return traverser.getDiscoveredNode();
    }

    NodeIndex Scene::searchNode(const std::string& nodeName, NodeIndex searchRootNode)
    {
      NodeSearchTraverser traverser(m_allocator, nodeName);

      traverser.doTraverse(m_allocator[searchRootNode]);

      return traverser.getDiscoveredNode(); 
    }

    NodeIndex Scene::addParentNode(NodeIndex destinationNodeIndex, const NodeIndex sourceNodeIndex)
    {
      TreeNode& destinationNode = m_allocator[destinationNodeIndex];
      NodeIndex newParentIndex = m_allocator.push_back(m_allocator[sourceNodeIndex]);
      GroupNode& newParent = (GroupNode&)m_allocator[newParentIndex];

      NodeIndex oldParentIndex = destinationNode.getParent();
      GroupNode& oldParent = (GroupNode&)m_allocator[oldParentIndex];

      newParent.setParent(oldParentIndex);
      newParent.setNextSibling(destinationNode.getNextSibling());
      newParent.setFirstChild(destinationNodeIndex);
      destinationNode.setParent(newParentIndex);
      destinationNode.setNextSibling(~0);

      if(oldParentIndex != ~0)
      {
        if(oldParent.getFirstChild() == destinationNodeIndex)//if the destination node was the first child, just change the parent
        {
          oldParent.setFirstChild(newParentIndex);
        }
        else//loop until we find the preceded sibling of the destination node and change its next sibling to the new node
        {
          NodeIndex firstSibling = oldParent.getFirstChild();
          NodeIndex oldSibling = ~0;
          while(firstSibling != destinationNodeIndex)
          {
            oldSibling = firstSibling;
            firstSibling = m_allocator[firstSibling].getNextSibling();
          }

          if(oldSibling != ~0)
          {
            m_allocator[oldSibling].setNextSibling(newParentIndex);
          }
        }
      }

      return newParentIndex;
    }

    NodeIndex Scene::addChildNode(NodeIndex destinationNodeIndex, const NodeIndex sourceNodeIndex)
    {
      GroupNode& destinationNode = (GroupNode&)m_allocator[destinationNodeIndex];
      NodeIndex oldFirstChildIndex = destinationNode.getFirstChild();

      NodeIndex newFirstChildIndex = m_allocator.push_back(m_allocator[sourceNodeIndex]);
      TreeNode& newFirstChild = m_allocator[newFirstChildIndex];

      destinationNode.setFirstChild(newFirstChildIndex);
      newFirstChild.setNextSibling(oldFirstChildIndex);
      newFirstChild.setParent(destinationNodeIndex);

      return newFirstChildIndex;
    }

    void Scene::removeNode(const NodeIndex node)
    {
      NodeIndex siblingIndex = m_allocator[node].getNextSibling();

      NodeIndex parentIndex = m_allocator[node].getParent();
      GroupNode& parent = (GroupNode&)m_allocator[parentIndex];

      NodeIndex firstChildIndex = m_allocator[node].getFirstChild();

      if(firstChildIndex != ~0)
      {
        parent.setFirstChild(firstChildIndex);

        TreeNode& firstChild = m_allocator[firstChildIndex];
        TreeNode& oldChildSibling = firstChild;
        while(firstChildIndex != ~0)
        {
          firstChild.setParent(parentIndex);

          oldChildSibling = firstChild;
          firstChildIndex = firstChild.getNextSibling();
        }

        oldChildSibling.setNextSibling(siblingIndex);
      }

      if(parent.getFirstChild() == node)
      {
        if(firstChildIndex != ~0)
        {
          parent.setFirstChild(firstChildIndex);
        }
        else
        {
          parent.setFirstChild(siblingIndex);
        }
      }
      else
      {
        NodeIndex parentFirstChildIndex = parent.getFirstChild();
        NodeIndex oldParentFirstChildindex = parentFirstChildIndex;
        while(parentFirstChildIndex != node)
        {
          oldParentFirstChildindex = parentFirstChildIndex;
          parentFirstChildIndex = m_allocator[parentFirstChildIndex].getNextSibling();
        }

        if(firstChildIndex != ~0)
        {
          m_allocator[oldParentFirstChildindex].setNextSibling(firstChildIndex);
        }
        else
        {
          m_allocator[oldParentFirstChildindex].setNextSibling(siblingIndex);
        }
      }

      m_allocator.erase(node);
    }

    NodeIndex Scene::addSubTree(const Scene& subTree, NodeIndex parentNodeIndex, const util::Vector<float, 3>& cameraPosition, std::string namePrefix)
    {
      CopyTraverser traverser(subTree.getTreeNodeAllocator(), m_allocator, namePrefix);
      traverser.doTraverse(subTree.getTreeNodeAllocator()[subTree.getRootNode()]);
      NodeIndex newNode = traverser.getCopiedRootNode();

      if(m_rootNode != ~0)
      {
        NodeIndex oldFirstChild = m_allocator[parentNodeIndex].getFirstChild();

        ((GroupNode&)m_allocator[parentNodeIndex]).setFirstChild(newNode);
        m_allocator[newNode].setNextSibling(oldFirstChild);
        m_allocator[newNode].setParent(parentNodeIndex);
      }
      else
      {
        m_rootNode = newNode;
      }

      return newNode;
    }

    void Scene::removeSubTree(const NodeIndex rootNodeIndex)
    {
      NodeIndex parentNodeIndex = m_allocator[rootNodeIndex].getParent();

      if(parentNodeIndex != ~0)
      {
        NodeIndex currentSibling = m_allocator[parentNodeIndex].getFirstChild();
        NodeIndex predSibling = currentSibling;

        //disconnect the subtree from the whole tree
        while(currentSibling != ~0)
        {
          if(currentSibling == rootNodeIndex)
          {
            if(currentSibling == predSibling)
            {
              ((GroupNode&)m_allocator[parentNodeIndex]).setFirstChild(m_allocator[currentSibling].getNextSibling());
            }
            else
            {
              m_allocator[predSibling].setNextSibling(m_allocator[currentSibling].getNextSibling());
            }

            break;//leave the loop
          }

          predSibling = currentSibling;
          currentSibling = m_allocator[currentSibling].getNextSibling();
        }
      }

      DeleteTraverser traverser(m_allocator);
      traverser.doTraverse(m_allocator[rootNodeIndex]);
    }

    TreeNodeAllocator& Scene::getTreeNodeAllocator()
    {
      return m_allocator;
    }

    const TreeNodeAllocator& Scene::getTreeNodeAllocator() const
    {
      return m_allocator;
    }

    void Scene::setAnimatedSceneTime(AnimatedTransformNode& node, float animationTime)
    {
      AnimationControlTraverser traverser(m_allocator);
      traverser.setAnimationTime(animationTime);
      traverser.doTraverse(node);
    }

    void Scene::addAnimatedSceneTime(AnimatedTransformNode& node, float animationTime)
    {
      AnimationControlTraverser traverser(m_allocator);
      traverser.addAnimationTime(animationTime);
      traverser.doTraverse(node);
    }

    void Scene::pauseAnimatedScene(AnimatedTransformNode& node, bool pauseAnimation)
    {
      AnimationControlTraverser traverser(m_allocator);
      traverser.setPauseAnimation(pauseAnimation);
      traverser.doTraverse(node);
    }

    void Scene::stopAnimatedScene(AnimatedTransformNode& node)
    {
      AnimationControlTraverser traverser(m_allocator);
      traverser.setStopAnimation();
      traverser.doTraverse(node);
    }
  }
}