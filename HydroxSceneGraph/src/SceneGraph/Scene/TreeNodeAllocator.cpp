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
    template<typename T> TreeNode* createTreeNode(TreeNode *adress, const TreeNode& node)
    {
      return new (reinterpret_cast<T*>(adress))T(node);
    }

    template<typename T> TreeNode* createTreeNode(TreeNode *adress, unsigned int offset)
    {
      return reinterpret_cast<T*>(adress) + offset;
    }

    TreeNodeAllocator::TreeNodeAllocator(unsigned int nodeBlockSize) : m_nodeBlockSize(nodeBlockSize)
    {
      m_treeNodeFactory.registerCreateFunction(GEONODE, createTreeNode<GeoNode>);
      m_treeNodeFactory.registerCreateFunction(ANIMATEDGEONODE, createTreeNode<AnimatedGeoNode>);
      m_treeNodeFactory.registerCreateFunction(TRANSFORMNODE, createTreeNode<TransformNode>);
      m_treeNodeFactory.registerCreateFunction(ANIMATEDTRANSFORMNODE, createTreeNode<AnimatedTransformNode>);
      m_treeNodeFactory.registerCreateFunction(BILLBOARDNODE, createTreeNode<BillboardNode>);
      m_treeNodeFactory.registerCreateFunction(LODNODE, createTreeNode<LODNode>);
      m_treeNodeFactory.registerCreateFunction(LIGHTNODE, createTreeNode<LightNode>);
      m_treeNodeFactory.registerCreateFunction(SHADOWLIGHTNODE, createTreeNode<ShadowLightNode>);
      m_treeNodeFactory.registerCreateFunction(PARTICLENODE, createTreeNode<ParticleNode>);

      m_nodeAddressConvert[GEONODE] = createTreeNode<GeoNode>;
      m_nodeAddressConvert[ANIMATEDGEONODE] = createTreeNode<AnimatedGeoNode>;
      m_nodeAddressConvert[TRANSFORMNODE] = createTreeNode<TransformNode>;
      m_nodeAddressConvert[ANIMATEDTRANSFORMNODE] = createTreeNode<AnimatedTransformNode>;
      m_nodeAddressConvert[BILLBOARDNODE] = createTreeNode<BillboardNode>;
      m_nodeAddressConvert[LODNODE] = createTreeNode<LODNode>;
      m_nodeAddressConvert[LIGHTNODE] = createTreeNode<LightNode>;
      m_nodeAddressConvert[SHADOWLIGHTNODE] = createTreeNode<ShadowLightNode>;
      m_nodeAddressConvert[PARTICLENODE] = createTreeNode<ParticleNode>;

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
      m_freeSlots = other.m_freeSlots;

      for(std::map<NodeType, std::vector<TreeNode*>>::const_iterator it = other.m_treeNodes.begin(); it != other.m_treeNodes.end(); it++)
      {
        m_treeNodes.insert(std::map<NodeType, std::vector<TreeNode*>>::value_type(it->first, std::vector<TreeNode*>(it->second.size(), nullptr)));

        for(unsigned int i = 0; i < it->second.size(); i++)
        {
          unsigned int blockSize = (i + 1) * m_nodeBlockSize < other.m_size.find(it->first)->second ? m_nodeBlockSize : other.m_size.find(it->first)->second % m_nodeBlockSize;
          for(unsigned int j = 0; j < blockSize; j++)
          {
            NodeIndex index = NodeIndex(i * m_nodeBlockSize + j, it->first);
            std::list<unsigned int>& list = m_freeSlots[index.nodeType];
            if(std::find(list.begin(), list.end(), index.index) == list.end())
            {
              this->insert(other[index], index);
            }
          }
        }
      }

      /*for(std::map<NodeType, std::vector<TreeNode*>>::iterator it = m_treeNodes.begin(); it != m_treeNodes.end(); it++)
      {
        for(unsigned int i = 0; i < it->second.size(); i++)
        {
          it->second[i] = (TreeNode*)std::malloc(m_nodeSizes[it->first] * m_nodeBlockSize);

          unsigned int blockSize = (i + 1) * m_nodeBlockSize < m_size[it->first] ? m_nodeBlockSize : m_size[it->first] % m_nodeBlockSize;
          for(unsigned int j = 0; j < blockSize; j++)
          {
            NodeIndex index = NodeIndex(i * m_nodeBlockSize + j, it->first);
            m_treeNodeFactory.createTreeNode(it->first, it->second[i] + j * m_nodeSizes[it->first], other[index]);
          }
        }
      }*/
    }

    TreeNodeAllocator::~TreeNodeAllocator()
    {
      for(std::map<NodeType, std::vector<TreeNode*>>::iterator it = m_treeNodes.begin(); it != m_treeNodes.end(); it++)
      {
        for(unsigned int i = 0; i < it->second.size(); i++)
        {
          unsigned int blockSize = (i + 1) * m_nodeBlockSize < m_size[it->first] ? m_nodeBlockSize : m_size[it->first] % m_nodeBlockSize;
          for(unsigned int j = 0; j < blockSize; j++)
          {
            NodeIndex index = NodeIndex(i * m_nodeBlockSize + j, it->first);

            std::list<unsigned int>::iterator lit = std::find(m_freeSlots[it->first].begin(), m_freeSlots[it->first].end(), index.index);

            if(lit == m_freeSlots[it->first].end())
            {
              (*this)[index].~TreeNode();
            }
          }
          std::free(m_treeNodes[it->first][i]);
        }
      }
    }

    TreeNodeAllocator& TreeNodeAllocator::operator=(TreeNodeAllocator other)
    {
      using namespace std;//for ADL
      swap(*this, other);

      return *this;
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
        index = m_size[type];
      }

      resize(NodeIndex(index, type));

      m_treeNodeFactory.createTreeNode(type, &(*this)[NodeIndex(index, type)], treeNode);
      (*this)[NodeIndex(index, type)].setNodeIndex(NodeIndex(index, type));

      return index;
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

    NodeIndex TreeNodeAllocator::push_back(const TreeNode& treeNode)
    {
      NodeType type = treeNode.getNodeType();
      NodeIndex index = NodeIndex(m_size[type]++, type);

      resize(index);
      TreeNode& tmp = (*this)[index];
      TreeNode* tmp2 = &tmp;
      m_treeNodeFactory.createTreeNode(type, &tmp, treeNode);
      (*this)[index].setNodeIndex(index);

      return index;
    }

    void TreeNodeAllocator::pop_back(NodeType type)
    {
      (*this)[NodeIndex(m_size[type]--, type)].~TreeNode();
    }

    TreeNode& TreeNodeAllocator::operator[](NodeIndex index)
    {
      unsigned int nodeBlock = index.index / m_nodeBlockSize;
      unsigned int offset = index.index % m_nodeBlockSize;
      TreeNode *tmp = m_nodeAddressConvert[index.nodeType](m_treeNodes[index.nodeType][nodeBlock], offset);
      return *tmp;
    }

    const TreeNode& TreeNodeAllocator::operator[](NodeIndex index) const
    {
      unsigned int nodeBlock = index.index / m_nodeBlockSize;
      unsigned int offset = m_nodeSizes.find(index.nodeType)->second * (index.index % m_nodeBlockSize);
      TreeNode *tmp = m_nodeAddressConvert.find(index.nodeType)->second(m_treeNodes.find(index.nodeType)->second[nodeBlock], offset);
      return *tmp;
    }

    /*NodeIndex TreeNodeAllocator::getIndex(const TreeNode& treeNode) const
    {
      unsigned int blockNumber = m_size.find(treeNode.getNodeType())->second / m_nodeBlockSize;
      for(unsigned int i = 0; i < blockNumber; i++)
      {
        for(unsigned int j = 0; j < m_nodeBlockSize; j++)
        {
          if(&(*this)[NodeIndex(i * m_nodeBlockSize + j, treeNode.getNodeType())] == &treeNode)
          {
            return NodeIndex(i * m_nodeBlockSize + j, treeNode.getNodeType());
          }
        }
      }

      return ~0;
    }*/

    void TreeNodeAllocator::reorderTreeNodes()
    {
      TreeNodeAllocator other(m_nodeBlockSize);

      for(std::map<NodeType, std::vector<TreeNode*>>::iterator it = m_treeNodes.begin(); it != m_treeNodes.end(); it++)
      {
        for(unsigned int i = 0; i < it->second.size(); i++)
        {
          unsigned int blockSize = (i + 1) * m_nodeBlockSize < other.m_size.find(it->first)->second ? m_nodeBlockSize : other.m_size.find(it->first)->second % m_nodeBlockSize;
          for(unsigned int j = 0; j < blockSize; j++)
          {
            other.push_back((*this)[NodeIndex(i * m_nodeBlockSize + j, it->first)]);
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

    void TreeNodeAllocator::writeToStream(std::ostream& ostream) const
    {
      ostream << m_nodeSizes.size() << std::endl;

      for(std::map<NodeType, unsigned int>::const_iterator it = m_nodeSizes.begin(); it != m_nodeSizes.end(); it++)
      {
        ostream << it->first << std::endl;
        ostream << it->second << std::endl;
      }

      for(std::map<NodeType, unsigned int>::const_iterator it = m_size.begin(); it != m_size.end(); it++)
      {
        ostream << it->second << std::endl;
      }

      for(std::map<NodeType, std::vector<TreeNode*>>::const_iterator it = m_treeNodes.begin(); it != m_treeNodes.end(); it++)
      {
        ostream << m_treeNodes.find(it->first)->second.size() << std::endl;
        for(unsigned int i = 0; i < m_treeNodes.find(it->first)->second.size(); i++)
        {
          ostream.write((const char*)m_treeNodes.find(it->first)->second[i], m_nodeSizes.find(it->first)->second * m_nodeBlockSize);
        }
      }
    }

    void TreeNodeAllocator::readFromStream(std::istream& istream)
    {
      unsigned int nodeTypeNumber = 0;
      istream >> nodeTypeNumber;

      std::vector<NodeType> nodeTypes(nodeTypeNumber);
      for(unsigned int i = 0; i < nodeTypeNumber; i++)
      {
        unsigned int nodeType = 0;
        istream >> nodeType;
        nodeTypes[i] = (NodeType)nodeType;

        unsigned int nodeTypeSize = 0;
        istream >> nodeTypeSize;
        m_nodeSizes.insert(std::map<NodeType, unsigned int>::value_type((NodeType)nodeType, nodeTypeSize));
      }

      for(unsigned int i = 0; i < nodeTypeNumber; i++)
      {
        unsigned int nodeSize = 0;
        istream >> nodeSize;
        m_size.insert(std::map<NodeType, unsigned int>::value_type(nodeTypes[i], nodeSize));
      }

      for(unsigned int i = 0; i < nodeTypeNumber; i++)
      {
        unsigned int nodeBlockNumber = 0;
        istream >> nodeBlockNumber;
        std::vector<TreeNode*> nodes(nodeBlockNumber);
        for(unsigned int j = 0; j < nodeBlockNumber; j++)
        {
          istream.read((char*)&nodes[j], m_nodeSizes[nodeTypes[i]] * m_nodeBlockSize);
        }
        m_treeNodes.insert(std::map<NodeType, std::vector<TreeNode*>>::value_type(nodeTypes[i], nodes));
      }
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
        }
      }
    }
  }
}