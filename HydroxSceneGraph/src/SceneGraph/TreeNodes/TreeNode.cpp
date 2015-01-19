#include "SceneGraph/TreeNodes/TreeNode.h"

#include <typeinfo>
#include <assert.h>

namespace he
{
  namespace sg
  {
    TreeNode::TreeNode(const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) : 
      m_parent(parent),
      m_nextSibling(nextSibling),
      m_nodeName(nodeName),
      m_index(~0)
    {
      m_index.nodeType = TREENODE;
    }

    TreeNode::TreeNode(const TreeNode& other)
    {
      m_parent = other.m_parent;
      m_nextSibling = other.m_nextSibling;
      m_nodeName = other.m_nodeName;
      m_index = other.m_index;
    }

    TreeNode::~TreeNode()
    {
    }

    NodeType TreeNode::getNodeType() const
    {
      return m_index.nodeType;
    }

    NodeIndex TreeNode::getNextSibling() const
    {
      return m_nextSibling;
    }

    NodeIndex TreeNode::getParent() const
    {
      return m_parent;
    }

    NodeIndex TreeNode::getFirstChild() const
    {
      return ~0;
    }

    void TreeNode::setNextSibling(NodeIndex nextSibling)
    {
      m_nextSibling = nextSibling;
    }

    void TreeNode::setParent(NodeIndex parent)
    {
      m_parent = parent;
    }

    void TreeNode::setNodeName(std::string& nodeName)
    {
      m_nodeName = nodeName;
    }

    const std::string& TreeNode::getNodeName() const
    {
      return m_nodeName;
    }

    void TreeNode::setNodeIndex(NodeIndex index)
    {
      m_index = index;
    }

    NodeIndex TreeNode::getNodeIndex() const
    {
      return m_index;
    }

    void TreeNode::read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {//nodeindex being read by the loader to know which type of node he has to create
      std::string name;
      std::getline(stream, m_nodeName);
      std::getline(stream, m_nodeName);//getline because of white spaces

      unsigned int nodeType;
      unsigned int nodeIndex;

      stream >> nodeType;
      stream >> nodeIndex;
      m_parent = NodeIndex(nodeIndex, (NodeType)nodeType);

      stream >> nodeType;
      stream >> nodeIndex;
      m_nextSibling = NodeIndex(nodeIndex, (NodeType)nodeType);
    }

    void TreeNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
    {
      stream << (unsigned int)m_index.nodeType << std::endl;
      stream << m_index.index << std::endl;

      stream << m_nodeName.c_str() << std::endl;

      stream << (unsigned int)m_parent.nodeType << std::endl;
      stream << m_parent.index << std::endl;

      stream << (unsigned int)m_nextSibling.nodeType << std::endl;
      stream << m_nextSibling.index << std::endl;
    }
  }
}