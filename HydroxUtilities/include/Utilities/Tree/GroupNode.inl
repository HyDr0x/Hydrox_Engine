#include "Utilities/Tree/ConstTraverser.h"
#include "Utilities/Tree/Traverser.h"

namespace he
{
  namespace util
  {
    template<class BaseTraverser> GroupNode<BaseTraverser>::GroupNode(const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling, NodeIndex firstChild) :
      TreeNode<BaseTraverser>(nodeName, parent, nextSibling), m_firstChild(firstChild)
    {
      m_dirtyFlag = DEFAULT;
      m_index.nodeType = GROUPNODE;
    }

    template<class BaseTraverser> GroupNode<BaseTraverser>::GroupNode(const GroupNode<BaseTraverser>& sourceNode) : TreeNode<BaseTraverser>(sourceNode)
    {
      m_firstChild = sourceNode.m_firstChild;
      m_dirtyFlag = DEFAULT;
    }

    template<class BaseTraverser> GroupNode<BaseTraverser>::GroupNode(const TreeNode<BaseTraverser>& sourceNode) : TreeNode<BaseTraverser>(sourceNode)
    {
      //assert(typeid(this) == typeid(sourceNode));

      const GroupNode<BaseTraverser>& copyNode = static_cast<const GroupNode<BaseTraverser>&>(sourceNode);

      new (this) GroupNode<BaseTraverser>(copyNode);
    }

    template<class BaseTraverser> GroupNode<BaseTraverser>::~GroupNode()
    {
    }

    template<class BaseTraverser> TreeNode<BaseTraverser>& GroupNode<BaseTraverser>::operator=(const TreeNode<BaseTraverser>& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~GroupNode<BaseTraverser>();

      new (this) GroupNode<BaseTraverser>(sourceNode);

      return *this;
    }

    template<class BaseTraverser> GroupNode<BaseTraverser>* GroupNode<BaseTraverser>::clone() const
    {
      GroupNode<BaseTraverser> *newNode = new GroupNode<BaseTraverser>(m_nodeName);

      newNode->m_nodeName = m_nodeName;
      newNode->m_dirtyFlag = m_dirtyFlag;

      return newNode;
    }

    template<class BaseTraverser> bool GroupNode<BaseTraverser>::ascendTraverse(Traverser<BaseTraverser>* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    template<class BaseTraverser> bool GroupNode<BaseTraverser>::preTraverse(Traverser<BaseTraverser>* traverser)
    {
      return traverser->preTraverse(*this);
    }

    template<class BaseTraverser> void GroupNode<BaseTraverser>::postTraverse(Traverser<BaseTraverser>* traverser)
    {
      traverser->postTraverse(*this);
    }

    template<class BaseTraverser> bool GroupNode<BaseTraverser>::ascendTraverse(ConstTraverser<BaseTraverser>* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    template<class BaseTraverser> bool GroupNode<BaseTraverser>::preTraverse(ConstTraverser<BaseTraverser>* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    template<class BaseTraverser> void GroupNode<BaseTraverser>::postTraverse(ConstTraverser<BaseTraverser>* traverser) const
    {
      traverser->postTraverse(*this);
    }

    template<class BaseTraverser> NodeIndex GroupNode<BaseTraverser>::getFirstChild() const
    {
      return m_firstChild;
    }

    template<class BaseTraverser> void GroupNode<BaseTraverser>::setFirstChild(NodeIndex firstChild)
    {
      m_firstChild = firstChild;
    }

    template<class BaseTraverser> DirtyFlags GroupNode<BaseTraverser>::getDirtyFlag() const
    {
      return (DirtyFlags)m_dirtyFlag;
    }

    template<class BaseTraverser> void GroupNode<BaseTraverser>::addDirtyFlag(DirtyFlags dirtyFlag)
    {
      m_dirtyFlag |= (unsigned int)dirtyFlag;
    }

    template<class BaseTraverser> void GroupNode<BaseTraverser>::removeDirtyFlag(DirtyFlags dirtyFlag)
    {
      m_dirtyFlag &= ~dirtyFlag;
    }

    template<class BaseTraverser> void GroupNode<BaseTraverser>::read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      TreeNode<BaseTraverser>::read(stream, eventManager, resourceHandles);

      unsigned int type;
      stream >> type;
      m_firstChild.nodeType = (NodeType)type;
      stream >> m_firstChild.index;

      m_dirtyFlag = DEFAULT;
    }

    template<class BaseTraverser> void GroupNode<BaseTraverser>::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
    {
      TreeNode<BaseTraverser>::write(stream, resourceHandles);

      stream << (unsigned int)m_firstChild.nodeType << std::endl;
      stream << m_firstChild.index << std::endl;
    }
  }
}