#include "SceneGraph/TreeNodes/ParticleNode.h"

#include <cassert>

#include <Utilities/Signals/EventManager.h>

#include <XBar/ParticleEmitterContainer.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {    ParticleNode::ParticleNode(util::SharedPointer<util::EventManager> eventManager, const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) :
      TreeNode(nodeName, parent, nextSibling),
      m_eventManager(eventManager)
    {
      m_index.nodeType = PARTICLENODE;
    }

    ParticleNode::ParticleNode(const ParticleNode& sourceNode) : TreeNode(sourceNode)
    {
      m_eventManager = sourceNode.m_eventManager;
    }

    ParticleNode::ParticleNode(const TreeNode& sourceNode) : TreeNode(sourceNode)
    {
      assert(PARTICLENODE == sourceNode.getNodeType());

      const ParticleNode& copyNode = static_cast<const ParticleNode&>(sourceNode);

      new (this) ParticleNode(copyNode);
    }

    ParticleNode::~ParticleNode()
    {
    }

    TreeNode& ParticleNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~ParticleNode();

      new (this) ParticleNode(sourceNode);

      return *this;
    }

    TreeNode* ParticleNode::clone() const
    {
      ParticleNode *newNode = new ParticleNode(m_eventManager, m_nodeName);

      newNode->m_nodeName = m_nodeName;

      newNode->m_trfMatrix = m_trfMatrix;

      return newNode;
    }

    bool ParticleNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    bool ParticleNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void ParticleNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool ParticleNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool ParticleNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void ParticleNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
    }

    void ParticleNode::setRenderable(bool renderable)
    {
      if (!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void(*)(const xBar::ParticleEmitterContainer& particleEmitter)>(util::EventManager::OnAddLightNode)->execute(xBar::ParticleEmitterContainer(&m_trfMatrix));
      }
      else if (m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void(*)(const xBar::ParticleEmitterContainer& particleEmitter)>(util::EventManager::OnRemoveLightNode)->execute(xBar::ParticleEmitterContainer(&m_trfMatrix));
      }

      m_renderable = renderable;
    }

    bool ParticleNode::getRenderable() const
    {
      return m_renderable;
    }

    void ParticleNode::setTransformationMatrix(const util::Matrix<float,4>& trfMatrix)
    {
      m_trfMatrix = trfMatrix;
    }

    util::Matrix<float,4> ParticleNode::getTransformationMatrix() const
    {
      return m_trfMatrix;
    }

    void ParticleNode::read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      TreeNode::read(stream, eventManager, resourceHandles);

      stream >> m_trfMatrix;

      m_eventManager = eventManager;
    }

    void ParticleNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
    {
      TreeNode::write(stream, resourceHandles);

      stream << m_trfMatrix << std::endl;
    }
  }
}