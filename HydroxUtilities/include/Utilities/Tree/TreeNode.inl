#include "Utilities/Tree/Traverser.h"
#include "Utilities/Tree/ConstTraverser.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> TreeNode<BaseTraverser>::TreeNode(const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) :
      m_parent(parent),
      m_nextSibling(nextSibling),
      m_nodeName(nodeName),
      m_index(~0)
    {
      m_index.nodeType = TREENODE;
    }

    template<class BaseTraverser> TreeNode<BaseTraverser>::TreeNode<BaseTraverser>(const TreeNode<BaseTraverser>& other)
    {
      m_parent = other.m_parent;
      m_nextSibling = other.m_nextSibling;
      m_nodeName = other.m_nodeName;
      m_index = other.m_index;
    }

    template<class BaseTraverser> TreeNode<BaseTraverser>::~TreeNode<BaseTraverser>()
    {
    }

    template<class BaseTraverser> NodeType TreeNode<BaseTraverser>::getNodeType() const
    {
      return m_index.nodeType;
    }

    template<class BaseTraverser> NodeIndex TreeNode<BaseTraverser>::getNextSibling() const
    {
      return m_nextSibling;
    }

    template<class BaseTraverser> NodeIndex TreeNode<BaseTraverser>::getParent() const
    {
      return m_parent;
    }

    template<class BaseTraverser> NodeIndex TreeNode<BaseTraverser>::getFirstChild() const
    {
      return ~0;
    }

    template<class BaseTraverser> void TreeNode<BaseTraverser>::setNextSibling(NodeIndex nextSibling)
    {
      m_nextSibling = nextSibling;
    }

    template<class BaseTraverser> void TreeNode<BaseTraverser>::setParent(NodeIndex parent)
    {
      m_parent = parent;
    }

    template<class BaseTraverser> void TreeNode<BaseTraverser>::setNodeName(std::string& nodeName)
    {
      m_nodeName = nodeName;
    }

    template<class BaseTraverser> const std::string& TreeNode<BaseTraverser>::getNodeName() const
    {
      return m_nodeName;
    }

    template<class BaseTraverser> void TreeNode<BaseTraverser>::setNodeIndex(NodeIndex index)
    {
      m_index = index;
    }

    template<class BaseTraverser> NodeIndex TreeNode<BaseTraverser>::getNodeIndex() const
    {
      return m_index;
    }

    template<class BaseTraverser> void TreeNode<BaseTraverser>::read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      std::string name;
      std::getline(stream, m_nodeName);
      std::getline(stream, m_nodeName);//getline because of white spaces

      unsigned int nodeTypes;
      unsigned int nodeIndex;

      stream >> nodeTypes;
      stream >> nodeIndex;
      m_parent = NodeIndex(nodeIndex, (NodeType)nodeTypes);

      stream >> nodeTypes;
      stream >> nodeIndex;
      m_nextSibling = NodeIndex(nodeIndex, (NodeType)nodeTypes);
    }

    template<class BaseTraverser> void TreeNode<BaseTraverser>::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
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