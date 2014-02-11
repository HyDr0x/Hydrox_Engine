#include "SceneGraph/TreeNodes/BillboardNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
	namespace sg
	{
    BillboardNode::BillboardNode(util::ResourceHandle billboardIndex, bool renderable, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                                                        m_billboardIndex(billboardIndex),
                                                                                                                                                        m_renderable(renderable)
    {
    }

    BillboardNode& BillboardNode::operator=(const BillboardNode& sourceNode)
    {
      TreeNode::operator=(sourceNode);

      m_trfMatrix = sourceNode.m_trfMatrix;
      m_billboardIndex = sourceNode.m_billboardIndex;
      m_renderable = sourceNode.m_renderable;

      return *this;
    }

    TreeNode& BillboardNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const BillboardNode& copyNode = static_cast<const BillboardNode&>(sourceNode);
      BillboardNode::operator=(copyNode);

      return *this;
    }

    BillboardNode::~BillboardNode()
    {
    }

    TreeNode* BillboardNode::clone() const
    {
      BillboardNode *newNode = new BillboardNode(m_billboardIndex, true, m_nodeName);

      newNode->m_nodeName = m_nodeName;

      newNode->m_trfMatrix = m_trfMatrix;
      newNode->m_renderable = m_renderable;

      return newNode;
    }

    bool BillboardNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(this);
    }

    bool BillboardNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void BillboardNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool BillboardNode::isBillboardNode()
    {
      return true;
    }

    util::ResourceHandle BillboardNode::getBillboardIndex() const
    {
      return m_billboardIndex;
    }

    util::Matrix<float,4> BillboardNode::getTransformationMatrix() const
    {
      return m_trfMatrix;
    }

    void BillboardNode::setTransformationMatrix(const util::Matrix<float,4>& trfMatrix)
    {
      m_trfMatrix = trfMatrix;
    }

    void BillboardNode::setRenderable(bool renderable)
    {
      m_renderable = renderable;
    }

    bool BillboardNode::getRenderable() const
    {
      return m_renderable;
    }
	}
}