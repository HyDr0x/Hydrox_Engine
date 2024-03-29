#include "SceneGraph/TreeNodes/ParticleNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

#include <XBar/ParticleContainer.h>

namespace he
{
	namespace sg
	{    ParticleNode::ParticleNode(const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling)
    {
    }

    ParticleNode& ParticleNode::operator=(const ParticleNode& sourceNode)
    {
      TreeNode::operator=(sourceNode);

      m_trfMatrix = sourceNode.m_trfMatrix;

      return *this;
    }

    TreeNode& ParticleNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const ParticleNode& copyNode = static_cast<const ParticleNode&>(sourceNode);
      ParticleNode::operator=(copyNode);

      return *this;
    }

    ParticleNode::~ParticleNode()
    {
    }

    TreeNode* ParticleNode::clone() const
    {
      ParticleNode *newNode = new ParticleNode(m_nodeName);

      newNode->m_nodeName = m_nodeName;

      newNode->m_trfMatrix = m_trfMatrix;

      return newNode;
    }

    bool ParticleNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(this);
    }

    bool ParticleNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void ParticleNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool ParticleNode::isParticleNode()
    {
      return true;
    }

    util::Matrix<float,4> ParticleNode::getTransformationMatrix() const
    {
      return m_trfMatrix;
    }

    void ParticleNode::setTransformationMatrix(const util::Matrix<float,4>& trfMatrix)
    {
      m_trfMatrix = trfMatrix;
    }
	}
}