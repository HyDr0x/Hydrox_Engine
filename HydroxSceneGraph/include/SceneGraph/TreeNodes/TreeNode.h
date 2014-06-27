#ifndef TREENODE_H_
#define TREENODE_H_

#include <string>

#include "SceneGraph/DLLExport.h"

namespace he
{
  namespace sg
  {    class ConstTraverser;
    class Traverser;
    class GroupNode;

    class GRAPHICAPI TreeNode
    {
    public:

      TreeNode(const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      virtual ~TreeNode() = 0;

      virtual TreeNode& operator=(const TreeNode& sourceNode);
  
      virtual TreeNode* clone() const = 0;
      
      virtual bool ascendTraverse(Traverser* traverser) = 0;
      virtual bool preTraverse(Traverser* traverser) = 0;
      virtual void postTraverse(Traverser* traverser) = 0;

      virtual bool ascendTraverse(ConstTraverser* traverser) const = 0;
      virtual bool preTraverse(ConstTraverser* traverser) const = 0;
      virtual void postTraverse(ConstTraverser* traverser) const = 0;

      virtual bool isTreeNode() const;
      virtual bool isTransformNode() const;
      virtual bool isParticleNode() const;
      virtual bool isLODNode() const;
      virtual bool isLightNode() const;
      virtual bool isShadowLightNode() const;
      virtual bool isGroupNode() const;
      virtual bool isGeoNode() const;
      virtual bool isBillboardNode() const;
      virtual bool isAnimatedTransformNode() const;
      virtual bool isAnimatedGeoNode() const;

      virtual TreeNode* getFirstChild() const;
      TreeNode* getNextSibling() const;
      GroupNode* getParent() const;

      void setNextSibling(TreeNode* nextSibling);
      void setParent(GroupNode* parent);

      void setNodeName(std::string& nodeName);
      const std::string& getNodeName() const;

    protected:

      std::string m_nodeName;

      GroupNode* m_parent;
      TreeNode* m_nextSibling;

    private:

      TreeNode(const TreeNode&);
    };
  }
}

#endif
