#include "SceneGraph/TreeNodes/LightNode.h"

#include <assert.h>

#include <Utilities/Signals/EventManager.h>

#include <XBar/LightContainer.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    LightNode::LightNode(util::EventManager *eventManager, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : 
      TreeNode(nodeName, parent, nextSibling), 
      m_eventManager(eventManager),
      m_renderable(false)
    {
    }

    LightNode::~LightNode()
    {
    }

    TreeNode& LightNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const LightNode& copyNode = static_cast<const LightNode&>(sourceNode);
      LightNode::operator=(copyNode);

      return *this;
    }

    TreeNode* LightNode::clone() const
    {
      LightNode *newNode = new LightNode(m_eventManager, m_nodeName);

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

    bool LightNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(this);
    }

    bool LightNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void LightNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    bool LightNode::isLightNode() const
    {
      return true;
    }

    void LightNode::setRenderable(bool renderable)
    {
      if(!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void(*)(const xBar::LightContainer& light)>(util::EventManager::OnAddLightNode)->execute(xBar::LightContainer(&m_trfMatrix, m_lightHandle));
      }
      else if (m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void(*)(const xBar::LightContainer& light)>(util::EventManager::OnRemoveLightNode)->execute(xBar::LightContainer(&m_trfMatrix, m_lightHandle));
      }

      m_renderable = renderable;
    }

    void LightNode::setLightHandle(util::ResourceHandle lightHandle)
    {
      m_lightHandle = lightHandle;
    }

    util::ResourceHandle LightNode::getLightHandle() const
    {
      return m_lightHandle;
    }

    void LightNode::setTransformationMatrix(const util::Matrix<float,4>& trfMatrix)
    {
      m_trfMatrix = trfMatrix;
    }

    util::Matrix<float,4> LightNode::getTransformationMatrix() const
    {
      return m_trfMatrix;
    }
  }
}