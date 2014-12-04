#ifndef GROUPNODE_H_
#define GROUPNODE_H_

#include "SceneGraph/DLLExport.h"

#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
  namespace sg
  {
    class Traverser;

    class GRAPHICAPI GroupNode : public TreeNode
    {
    public:

      enum DirtyFlags
      {
        DEFAULT     = 0,
        TRF_DIRTY   = 1,
        BV_DIRTY    = 2,
        LOD_INRANGE = 4,
        ANIM_DIRTY  = 8,
      };

      GroupNode(){}
      GroupNode(const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0, NodeIndex firstChild = ~0);
      GroupNode(const GroupNode& sourceNode);
      GroupNode(const TreeNode& sourceNode);
      virtual ~GroupNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual GroupNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      NodeIndex getFirstChild() const;

      void setFirstChild(NodeIndex firstChild);

      DirtyFlags getDirtyFlag() const;
      void addDirtyFlag(DirtyFlags dirtyFlag);
      void removeDirtyFlag(DirtyFlags dirtyFlag);

      virtual void read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const;

    protected:

      NodeIndex m_firstChild;

      unsigned int m_dirtyFlag;
    };
  }
}

#endif
