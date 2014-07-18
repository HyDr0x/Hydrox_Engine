#ifndef TREENODE_H_
#define TREENODE_H_

#include <string>

#include "SceneGraph/DLLExport.h"

namespace he
{
  namespace sg
  {    enum NodeType
    {
      TREENODE,
      GROUPNODE,
      GEONODE,
      ANIMATEDGEONODE,
      TRANSFORMNODE,
      ANIMATEDTRANSFORMNODE,
      BILLBOARDNODE,
      LODNODE,
      LIGHTNODE,
      SHADOWLIGHTNODE,
      PARTICLENODE,
    };    class ConstTraverser;
    class Traverser;
    class GroupNode;

    struct NodeIndex
    {
      class Less
      {
      public:
        inline bool operator()(const NodeIndex& o1, const NodeIndex& o2) const
        {
          return o1.index < o2.index;
        }
      };

      NodeIndex(unsigned int newIndex = ~0, NodeType newNodeType = TREENODE) : index(newIndex), nodeType(newNodeType)
      {
      }

      bool operator==(const NodeIndex& other)
      {
        return index == other.index && nodeType == other.nodeType;
      }

      bool operator!=(const NodeIndex& other)
      {
        return index != other.index || nodeType != other.nodeType;
      }

      unsigned int index;
      NodeType nodeType;
    };

    class GRAPHICAPI TreeNode
    {
    public:

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

    protected:

      std::string m_nodeName;

      NodeIndex m_index;

      NodeIndex m_parent;
      NodeIndex m_nextSibling;
    };
  }
}

#endif
