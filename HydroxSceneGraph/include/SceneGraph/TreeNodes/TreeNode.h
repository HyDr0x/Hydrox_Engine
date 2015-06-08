#ifndef TREENODE_H_
#define TREENODE_H_

#include <string>
#include <stdint.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>

#include "SceneGraph/DLLExport.h"
#include "SceneGraph/TreeNodes/NodeIndex.h"

namespace he
{
  namespace sg
  {
    class GRAPHICAPI TreeNode
    {
    public:

      TreeNode(){}
      TreeNode(const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0);
      TreeNode(const TreeNode& other);
      virtual ~TreeNode() = 0;
  
      virtual TreeNode* clone() const = 0;
      
      virtual bool ascendTraverse(Traverser* traverser) = 0;
      virtual bool preTraverse(Traverser* traverser) = 0;
      virtual void postTraverse(Traverser* traverser) = 0;

      virtual bool ascendTraverse(ConstTraverser* traverser) const = 0;
      virtual bool preTraverse(ConstTraverser* traverser) const = 0;
      virtual void postTraverse(ConstTraverser* traverser) const = 0;

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

      virtual void read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::ResourceHandle::Less>>& resourceHandles) const;

    protected:

      std::string m_nodeName;

      NodeIndex m_index;

      NodeIndex m_parent;
      NodeIndex m_nextSibling;
    };
  }
}

#endif
