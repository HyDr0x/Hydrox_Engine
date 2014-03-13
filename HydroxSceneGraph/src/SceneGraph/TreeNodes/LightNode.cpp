#include "SceneGraph/TreeNodes/LightNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

#include "SceneGraph/XBarContainer/LightContainer.h"

namespace he
{
	namespace sg
	{
    LightNode::LightNode(const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling)
    {
    }

    LightNode& LightNode::operator=(const LightNode& sourceNode)
    {
      TreeNode::operator=(sourceNode);

      m_trfMatrix = sourceNode.m_trfMatrix;

      return *this;
    }

    TreeNode& LightNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const LightNode& copyNode = static_cast<const LightNode&>(sourceNode);
      LightNode::operator=(copyNode);

      return *this;
    }

    LightNode::~LightNode()
    {
    }

    TreeNode* LightNode::clone() const
    {
      LightNode *newNode = new LightNode(m_nodeName);

      newNode->m_nodeName = m_nodeName;

      newNode->m_trfMatrix = m_trfMatrix;

      return newNode;
    }

    bool LightNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(this);
    }

    bool LightNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void LightNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool LightNode::isLightNode()
    {
      return true;
    }

    util::Matrix<float,4> LightNode::getTransformationMatrix() const
    {
      return m_trfMatrix;
    }

    void LightNode::setTransformationMatrix(const util::Matrix<float,4>& trfMatrix)
    {
      m_trfMatrix = trfMatrix;
    }
	}
}