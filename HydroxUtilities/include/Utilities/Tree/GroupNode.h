#ifndef UGROUPNODE_H_
#define UGROUPNODE_H_

#include <cassert>

#include "Utilities/Tree/TreeNode.h"

namespace he
{
  namespace util
  {
    enum DirtyFlags
    {
      DEFAULT = 0,
      DIRTYSTARTCOUNTER
    };

    template<class BaseTraverser> class Traverser;
    template<class BaseTraverser> class ConstTraverser;

    template<class BaseTraverser> class GroupNode : public TreeNode<BaseTraverser>
    {
    public:

      GroupNode(){}
      GroupNode(const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0, NodeIndex firstChild = ~0);
      GroupNode(const GroupNode<BaseTraverser>& sourceNode);
      GroupNode(const TreeNode<BaseTraverser>& sourceNode);
      virtual ~GroupNode();

      virtual TreeNode<BaseTraverser>& operator=(const TreeNode& sourceNode);

      virtual GroupNode<BaseTraverser>* clone() const;

      virtual bool ascendTraverse(Traverser<BaseTraverser>* traverser);
      virtual bool preTraverse(Traverser<BaseTraverser>* traverser);
      virtual void postTraverse(Traverser<BaseTraverser>* traverser);

      virtual bool ascendTraverse(ConstTraverser<BaseTraverser>* traverser) const;
      virtual bool preTraverse(ConstTraverser<BaseTraverser>* traverser) const;
      virtual void postTraverse(ConstTraverser<BaseTraverser>* traverser) const;

      NodeIndex getFirstChild() const;

      void setFirstChild(NodeIndex firstChild);

      DirtyFlags getDirtyFlag() const;
      void addDirtyFlag(DirtyFlags dirtyFlag);
      void removeDirtyFlag(DirtyFlags dirtyFlag);

      virtual void read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const;

    protected:

      NodeIndex m_firstChild;

      unsigned int m_dirtyFlag;
    };
  }
}

#include "Utilities/Tree/GroupNode.inl"

#endif
