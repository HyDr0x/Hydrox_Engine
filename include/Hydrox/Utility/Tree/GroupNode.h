#ifndef GROUPNODE_H_
#define GROUPNODE_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Pointer/SharedPointer.hpp"
#include "Hydrox/Utility/Tree/TreeNode.h"

namespace he
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

    GroupNode(const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
    GroupNode& operator=(const GroupNode& sourceNode);
    virtual TreeNode& operator=(const TreeNode& sourceNode);
    virtual ~GroupNode();

    virtual GroupNode* clone() const;

    virtual bool ascendTraverse(Traverser* traverser);
    virtual bool preTraverse(Traverser* traverser);
    virtual void postTraverse(Traverser* traverser);

    TreeNode* getFirstChild() const;

    void setFirstChild(TreeNode* firstChild);

    DirtyFlags getDirtyFlag() const;
    void addDirtyFlag(DirtyFlags dirtyFlag);
    void removeDirtyFlag(DirtyFlags dirtyFlag);

  protected:

    TreeNode* m_firstChild;

    unsigned int m_dirtyFlag;
  };
}

#endif
