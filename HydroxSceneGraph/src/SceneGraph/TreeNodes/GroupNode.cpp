#include "SceneGraph/TreeNodes/GroupNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    GroupNode::GroupNode(const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling, NodeIndex firstChild) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                        m_firstChild(firstChild)
    {
      m_dirtyFlag = DEFAULT;
      m_index.nodeType = GROUPNODE;
    }

    GroupNode::GroupNode(const GroupNode& sourceNode) : TreeNode(sourceNode)
    {
      m_firstChild = sourceNode.m_firstChild;
      m_dirtyFlag = DEFAULT;
    }

    GroupNode::GroupNode(const TreeNode& sourceNode) : TreeNode(sourceNode)
    {
      //assert(typeid(this) == typeid(sourceNode));

      const GroupNode& copyNode = static_cast<const GroupNode&>(sourceNode);

      new (this) GroupNode(copyNode);
    }

    GroupNode::~GroupNode()
    {
    }

    TreeNode& GroupNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~GroupNode();

      new (this) GroupNode(sourceNode);

      return *this;
    }

    GroupNode* GroupNode::clone() const
    {
      GroupNode *newNode = new GroupNode(m_nodeName);

      newNode->m_nodeName = m_nodeName;
      newNode->m_dirtyFlag = m_dirtyFlag;

      return newNode;
    }

    bool GroupNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    bool GroupNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void GroupNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool GroupNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool GroupNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void GroupNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
    }

    NodeIndex GroupNode::getFirstChild() const
    {
      return m_firstChild;
    }

    void GroupNode::setFirstChild(NodeIndex firstChild)
    {
      m_firstChild = firstChild;
    }

    GroupNode::DirtyFlags GroupNode::getDirtyFlag() const
    {
      return (DirtyFlags)m_dirtyFlag;
    }

    void GroupNode::addDirtyFlag(DirtyFlags dirtyFlag)
    {
      m_dirtyFlag |= (unsigned int)dirtyFlag;
    }

    void GroupNode::removeDirtyFlag(DirtyFlags dirtyFlag)
    {
      m_dirtyFlag &= ~dirtyFlag;
    }
  }
}