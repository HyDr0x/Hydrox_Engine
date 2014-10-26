#ifndef UTREENODE_H_
#define UTREENODE_H_

#include <string>
#include <stdint.h>
#include <typeinfo>
#include <cassert>

#include "Utilities/Miscellaneous/SingletonManager.hpp"
#include "Utilities/Miscellaneous/ResourceHandle.h"

#include "Utilities/Signals/EventManager.h"

#include "Utilities/Tree/NodeIndex.h"


namespace he
{
  namespace util
  {    template<class BaseTraverser> class Traverser;
    template<class BaseTraverser> class ConstTraverser;    template<class BaseTraverser> class TreeNode
    {
    public:

      TreeNode(){}
      TreeNode(const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0);
      TreeNode(const TreeNode<BaseTraverser>& other);
      virtual ~TreeNode() = 0;
  
      virtual TreeNode<BaseTraverser>* clone() const = 0;
      
      virtual bool ascendTraverse(Traverser<BaseTraverser>* traverser) = 0;
      virtual bool preTraverse(Traverser<BaseTraverser>* traverser) = 0;
      virtual void postTraverse(Traverser<BaseTraverser>* traverser) = 0;

      virtual bool ascendTraverse(ConstTraverser<BaseTraverser>* traverser) const = 0;
      virtual bool preTraverse(ConstTraverser<BaseTraverser>* traverser) const = 0;
      virtual void postTraverse(ConstTraverser<BaseTraverser>* traverser) const = 0;

      virtual NodeType getNodeType() const;

      virtual NodeIndex getFirstChild() const;
      NodeIndex getNextSibling() const;
      NodeIndex getParent() const;

      void setNextSibling(NodeIndex nextSibling);
      void setParent(NodeIndex parent);

      void setNodeName(std::string& nodeName);
      const std::string& getNodeName() const;

      void setNodeIndex(NodeIndex index);
      NodeIndex getNodeIndex() const;

      virtual void read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const;

    protected:

      std::string m_nodeName;

      NodeIndex m_index;

      NodeIndex m_parent;
      NodeIndex m_nextSibling;
    };
  }
}

#include "Utilities/Tree/TreeNode.inl"

#endif
