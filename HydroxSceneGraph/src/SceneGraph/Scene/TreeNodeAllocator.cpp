#include "SceneGraph/Scene/TreeNodeAllocator.h"

#include <assert.h>
#include <algorithm>
#include <new>

#include "SceneGraph/Traverser/DeleteTraverser.h"
#include "SceneGraph/Traverser/CopyTraverser.h"
#include "SceneGraph/Traverser/NodeSearchTraverser.h"
#include "SceneGraph/Traverser/AnimationControlTraverser.h"

#include "SceneGraph/TreeNodes/TreeNode.h"
#include "SceneGraph/TreeNodes/GroupNode.h"
#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"
#include "SceneGraph/TreeNodes/ShadowLightNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"

namespace he
{
  namespace sg
  {
    template<typename T> void createBlankTreeNode(TreeNode *adress)
    {
      new (reinterpret_cast<T*>(adress))T();
    }

    template<typename T> void createTreeNode(TreeNode *adress, const TreeNode& node)
    {
      new (reinterpret_cast<T*>(adress))T(node);
    }

    template<typename T> TreeNode* getTreeNode(TreeNode *adress, unsigned int offset)
    {
      return reinterpret_cast<T*>(adress) + offset;
    }

    TreeNodeAllocator::TreeNodeAllocator(unsigned int nodeBlockSize) : m_nodeBlockSize(nodeBlockSize)
    {
      m_nodeAddressConvert.resize(NODENUMBER);
      m_nodeSizes.resize(NODENUMBER);
      m_freeSlots.resize(NODENUMBER);
      m_treeNodes.resize(NODENUMBER);
        
      m_treeNodeFactory.registerCreateFunction(GROUPNODE, createBlankTreeNode<GroupNode>);
      m_treeNodeFactory.registerCreateFunction(GEONODE, createBlankTreeNode<GeoNode>);
      m_treeNodeFactory.registerCreateFunction(ANIMATEDGEONODE, createBlankTreeNode<AnimatedGeoNode>);
      m_treeNodeFactory.registerCreateFunction(TRANSFORMNODE, createBlankTreeNode<TransformNode>);
      m_treeNodeFactory.registerCreateFunction(ANIMATEDTRANSFORMNODE, createBlankTreeNode<AnimatedTransformNode>);
      m_treeNodeFactory.registerCreateFunction(BILLBOARDNODE, createBlankTreeNode<BillboardNode>);
      m_treeNodeFactory.registerCreateFunction(LODNODE, createBlankTreeNode<LODNode>);
      m_treeNodeFactory.registerCreateFunction(LIGHTNODE, createBlankTreeNode<LightNode>);
      m_treeNodeFactory.registerCreateFunction(SHADOWLIGHTNODE, createBlankTreeNode<ShadowLightNode>);
      m_treeNodeFactory.registerCreateFunction(PARTICLENODE, createBlankTreeNode<ParticleNode>);

      m_treeNodeFactory.registerCreateFunction(GROUPNODE, createTreeNode<GroupNode>);
      m_treeNodeFactory.registerCreateFunction(GEONODE, createTreeNode<GeoNode>);
      m_treeNodeFactory.registerCreateFunction(ANIMATEDGEONODE, createTreeNode<AnimatedGeoNode>);
      m_treeNodeFactory.registerCreateFunction(TRANSFORMNODE, createTreeNode<TransformNode>);
      m_treeNodeFactory.registerCreateFunction(ANIMATEDTRANSFORMNODE, createTreeNode<AnimatedTransformNode>);
      m_treeNodeFactory.registerCreateFunction(BILLBOARDNODE, createTreeNode<BillboardNode>);
      m_treeNodeFactory.registerCreateFunction(LODNODE, createTreeNode<LODNode>);
      m_treeNodeFactory.registerCreateFunction(LIGHTNODE, createTreeNode<LightNode>);
      m_treeNodeFactory.registerCreateFunction(SHADOWLIGHTNODE, createTreeNode<ShadowLightNode>);
      m_treeNodeFactory.registerCreateFunction(PARTICLENODE, createTreeNode<ParticleNode>);

      m_nodeAddressConvert[GROUPNODE] = getTreeNode<GroupNode>;
      m_nodeAddressConvert[GEONODE] = getTreeNode<GeoNode>;
      m_nodeAddressConvert[ANIMATEDGEONODE] = getTreeNode<AnimatedGeoNode>;
      m_nodeAddressConvert[TRANSFORMNODE] = getTreeNode<TransformNode>;
      m_nodeAddressConvert[ANIMATEDTRANSFORMNODE] = getTreeNode<AnimatedTransformNode>;
      m_nodeAddressConvert[BILLBOARDNODE] = getTreeNode<BillboardNode>;
      m_nodeAddressConvert[LODNODE] = getTreeNode<LODNode>;
      m_nodeAddressConvert[LIGHTNODE] = getTreeNode<LightNode>;
      m_nodeAddressConvert[SHADOWLIGHTNODE] = getTreeNode<ShadowLightNode>;
      m_nodeAddressConvert[PARTICLENODE] = getTreeNode<ParticleNode>;

      m_nodeSizes[GROUPNODE] = sizeof(GroupNode);
      m_nodeSizes[GEONODE] = sizeof(GeoNode);
      m_nodeSizes[ANIMATEDGEONODE] = sizeof(AnimatedGeoNode);
      m_nodeSizes[TRANSFORMNODE] = sizeof(TransformNode);
      m_nodeSizes[ANIMATEDTRANSFORMNODE] = sizeof(AnimatedTransformNode);
      m_nodeSizes[BILLBOARDNODE] = sizeof(BillboardNode);
      m_nodeSizes[LODNODE] = sizeof(LODNode);
      m_nodeSizes[LIGHTNODE] = sizeof(LightNode);
      m_nodeSizes[SHADOWLIGHTNODE] = sizeof(ShadowLightNode);
      m_nodeSizes[PARTICLENODE] = sizeof(ParticleNode);
    }

    TreeNodeAllocator::TreeNodeAllocator(const TreeNodeAllocator& other)
    {
      m_nodeBlockSize = other.m_nodeBlockSize;
      m_treeNodeFactory = other.m_treeNodeFactory;
      m_nodeAddressConvert = other.m_nodeAddressConvert;
      m_nodeSizes = other.m_nodeSizes;

      m_freeSlots.resize(NODENUMBER);
      m_treeNodes.resize(NODENUMBER);

      for(unsigned int k = 0; k < other.m_treeNodes.size(); k++)
      {
        for(unsigned int i = 0; i < other.m_treeNodes[k].size(); i++)
        {
          for(unsigned int j = 0; j < m_nodeBlockSize; j++)
          {
            NodeIndex index = NodeIndex(i * m_nodeBlockSize + j, (NodeType)k);
            const std::list<unsigned int>& list = other.m_freeSlots[index.nodeType];
            if(std::find(list.begin(), list.end(), index.index) == list.end())
            {
              this->insert(other[index], index);
            }
          }
        }
      }
    }

    TreeNodeAllocator::~TreeNodeAllocator()
    {
      for(unsigned int k = 0; k < m_treeNodes.size(); k++)
      {
        for(unsigned int i = 0; i < m_treeNodes[k].size(); i++)
        {
          for(unsigned int j = 0; j < m_nodeBlockSize; j++)
          {
            NodeIndex index = NodeIndex(i * m_nodeBlockSize + j, (NodeType)k);

            std::list<unsigned int>::iterator lit = std::find(m_freeSlots[(NodeType)k].begin(), m_freeSlots[(NodeType)k].end(), index.index);

            if(lit == m_freeSlots[(NodeType)k].end())
            {
              (*this)[index].~TreeNode();
            }
          }
          std::free(m_treeNodes[(NodeType)k][i]);
        }
      }
    }

    TreeNodeAllocator& TreeNodeAllocator::operator=(TreeNodeAllocator other)
    {
      using namespace std;//for ADL
      swap(*this, other);

      return *this;
    }

    TreeNode& TreeNodeAllocator::insert(NodeIndex index)
    {
      resize(index);

      m_freeSlots[index.nodeType].remove(index.index);

      TreeNode *newNode = &(*this)[index];

      m_treeNodeFactory.createTreeNode(index.nodeType, newNode);
      newNode->setNodeIndex(index);

      return *newNode;
    }

    NodeIndex TreeNodeAllocator::insert(const TreeNode& treeNode)
    {
      NodeType type = treeNode.getNodeType();
      unsigned int index = ~0;

      if(!m_freeSlots[type].empty())
      {
        index = m_freeSlots[type].front();
        m_freeSlots[type].pop_front();
      }
      else
      {
        resize(NodeIndex(m_treeNodes[type].size() * m_nodeBlockSize, type));
        index = m_freeSlots[type].front();
        m_freeSlots[type].pop_front();
      }

      m_treeNodeFactory.createTreeNode(type, &(*this)[NodeIndex(index, type)], treeNode);
      (*this)[NodeIndex(index, type)].setNodeIndex(NodeIndex(index, type));

      return NodeIndex(index, type);
    }

    TreeNode& TreeNodeAllocator::insert(const TreeNode& treeNode, NodeIndex index)
    {
      NodeType type = treeNode.getNodeType();

      resize(index);

      m_freeSlots[type].remove(index.index);
      
      TreeNode *newNode = &(*this)[index];

      m_treeNodeFactory.createTreeNode(type, newNode, treeNode);
      newNode->setNodeIndex(index);

      return *newNode;
    }

    void TreeNodeAllocator::erase(NodeIndex index)
    {
      (*this)[index].~TreeNode();
      m_freeSlots[index.nodeType].push_back(index.index);
    }

    TreeNode& TreeNodeAllocator::operator[](NodeIndex index)
    {
      unsigned int nodeBlock = index.index / m_nodeBlockSize;
      return *m_nodeAddressConvert[index.nodeType](m_treeNodes[index.nodeType][nodeBlock], index.index % m_nodeBlockSize);
    }

    const TreeNode& TreeNodeAllocator::operator[](NodeIndex index) const
    {
      unsigned int nodeBlock = index.index / m_nodeBlockSize;
      return *m_nodeAddressConvert[index.nodeType](m_treeNodes[index.nodeType][nodeBlock], index.index % m_nodeBlockSize);
    }

    bool TreeNodeAllocator::contains(NodeIndex index) const
    {
      return std::find(m_freeSlots[index.nodeType].begin(), m_freeSlots[index.nodeType].end(), index.index) == m_freeSlots[index.nodeType].end();
    }

    void TreeNodeAllocator::reorderTreeNodes()
    {
      TreeNodeAllocator other(m_nodeBlockSize);

      for(unsigned int k = 0; k < m_treeNodes.size(); k++)
      {
        for(unsigned int i = 0; i < m_treeNodes[k].size(); i++)
        {
          for(unsigned int j = 0; j < m_nodeBlockSize; j++)
          {
            other.insert((*this)[NodeIndex(i * m_nodeBlockSize + j, (NodeType)k)]);
          }
        }
      }

      using namespace std;//using ADL
      swap(*this, other);//copy swap idiom

      //for(std::map<NodeType, std::vector<TreeNode*>>::iterator it = m_treeNodes.begin(); it != m_treeNodes.end(); it++)
      //{
      //  unsigned int blockNumber = m_size[it->first] / m_nodeBlockSize;
      //  unsigned int offset = 0;
      //  bool finished = false;

      //  std::vector<TreeNode*> newNodes((m_size[it->first] - m_freeSlots[it->first].size()) / m_nodeBlockSize + 1);

      //  for(unsigned int i = 0; i < newNodes.size(); i++)
      //  {
      //    newNodes[i] = (TreeNode*)std::malloc(m_nodeSizes[it->first] * m_nodeBlockSize);
      //  }

      //  for(unsigned int i = 0; i < blockNumber; i++)
      //  {
      //    for(unsigned int j = 0; j < m_nodeBlockSize; j++)
      //    {
      //      do//go through the list of free indices until we are reaching another non free index from where we can copy a node
      //      {
      //        std::list<unsigned int>::iterator lit = std::find(m_freeSlots[it->first].begin(), m_freeSlots[it->first].end(), i * m_nodeBlockSize + j + offset);

      //        if(lit != m_freeSlots[it->first].end())
      //        {
      //          offset++;
      //        }
      //        else
      //        {
      //          break;
      //        }
      //      } while(true);

      //      unsigned int newIndex = i * m_nodeBlockSize + j + offset;//calculate the read index with an offset

      //      if(newIndex >= m_size[it->first])//the old node list reached the end, no more old nodes to copy
      //      {
      //        finished = true;
      //        break;
      //      }

      //      *(newNodes[i] + m_nodeSizes[it->first] * j) = (*this)[NodeIndex(newIndex, it->first)];//copy the treenode
      //      (newNodes[i] + m_nodeSizes[it->first] * j)->setNodeIndex(NodeIndex(i * m_nodeBlockSize + j, it->first));//sets the new nodeIndex
      //      (*this)[NodeIndex(newIndex, it->first)].~TreeNode();//calls the DTor of the treenode
      //    }

      //    if(finished)
      //    {
      //      break;
      //    }
      //  }

      //  for(unsigned int i = 0; i < m_treeNodes[it->first].size(); i++)
      //  {
      //    std::free(m_treeNodes[it->first][i]);
      //  }

      //  m_treeNodes[it->first] = newNodes;
      //  m_size[it->first] -= m_freeSlots[it->first].size();
      //  m_freeSlots[it->first].clear();
      //}
    }

    void TreeNodeAllocator::resize(NodeIndex index)
    {
      unsigned int newNodeBlockNumber = index.index / m_nodeBlockSize + 1;
      unsigned int oldNodeBlockNumber = m_treeNodes[index.nodeType].size();

      if(oldNodeBlockNumber < newNodeBlockNumber)
      {
        m_treeNodes[index.nodeType].resize(newNodeBlockNumber);

        for(unsigned int i = oldNodeBlockNumber; i < newNodeBlockNumber; i++)
        {
          m_treeNodes[index.nodeType][i] = (TreeNode*)std::malloc(m_nodeSizes[index.nodeType] * m_nodeBlockSize);
          for(unsigned int j = 0; j < m_nodeBlockSize; j++)
          {
            m_freeSlots[index.nodeType].push_back(j + oldNodeBlockNumber * m_nodeBlockSize);
          }
        }
      }
    }

    unsigned int TreeNodeAllocator::getNodeBlockSize() const
    {
      return m_nodeBlockSize;
    }

    unsigned int TreeNodeAllocator::getExactNodeBlockSize(NodeType type) const
    {
      return (m_treeNodes[type].size() * m_nodeBlockSize) - m_freeSlots[type].size();
    }

    unsigned int TreeNodeAllocator::getNodeBlockNumber(NodeType type) const
    {
      return m_treeNodes[type].size();
    }
  }
}