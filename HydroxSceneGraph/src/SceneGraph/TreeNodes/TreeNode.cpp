#include "SceneGraph/TreeNodes/TreeNode.h"

#include <typeinfo>
#include <assert.h>

namespace he
{
	namespace sg
	{
    TreeNode::TreeNode(const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : m_parent(parent),
                                                                                                m_nextSibling(nextSibling),
                                                                                                m_nodeName(nodeName)
    {
    }

    TreeNode& TreeNode::operator=(const TreeNode& sourceNode)
    {
      m_parent = sourceNode.m_parent;
      m_nextSibling = sourceNode.m_nextSibling;
      m_nodeName = sourceNode.m_nodeName;

      return *this;
    }

    TreeNode::~TreeNode()
    {
      m_nextSibling = nullptr;
      m_parent = nullptr;
    }

    bool TreeNode::isTreeNode() const
    {
      return true;
    }

    bool TreeNode::isTransformNode() const
    {
      return false;
    }

    bool TreeNode::isParticleNode() const
    {
      return false;
    }

    bool TreeNode::isLODNode() const
    {
      return false;
    }

    bool TreeNode::isLightNode() const
    {
      return false;
    }

    bool TreeNode::isGroupNode() const
    {
      return false;
    }

    bool TreeNode::isGeoNode() const
    {
      return false;
    }

    bool TreeNode::isBillboardNode() const
    {
      return false;
    }

    bool TreeNode::isAnimatedTransformNode() const
    {
      return false;
    }

    bool TreeNode::isAnimatedGeoNode() const
    {
      return false;
    }

    TreeNode* TreeNode::getNextSibling() const
    {
      return m_nextSibling;
    }

    GroupNode* TreeNode::getParent() const
    {
      return m_parent;
    }

    TreeNode* TreeNode::getFirstChild() const
    {
      return nullptr;
    }

    void TreeNode::setNextSibling(TreeNode* nextSibling)
    {
      m_nextSibling = nextSibling;
    }

    void TreeNode::setParent(GroupNode* parent)
    {
      m_parent = parent;
    }

    void TreeNode::setNodeName(std::string& nodeName)
    {
      m_nodeName = nodeName;
    }

    const std::string& TreeNode::getNodeName() const
    {
      return m_nodeName;
    }
	}
}