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
    LightNode::LightNode(LightType lightType, util::EventManager *eventManager, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) :
      TreeNode(nodeName, parent, nextSibling), 
      m_lightType(lightType),
      m_eventManager(eventManager),
      m_position(util::Vector<float, 4>::identity()),
      m_direction(util::Vector<float, 4>::identity()),
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
      LightNode *newNode = new LightNode(m_lightType, m_eventManager, m_nodeName);

      newNode->m_nodeName = m_nodeName;

      newNode->m_position = m_position;
      newNode->m_direction = m_direction;

      newNode->m_lightData = m_lightData;

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
        m_eventManager->raiseSignal<void(*)(const xBar::LightContainer& light)>(util::EventManager::OnAddLightNode)->execute(xBar::LightContainer(m_lightData));
      }
      else if (m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void(*)(const xBar::LightContainer& light)>(util::EventManager::OnRemoveLightNode)->execute(xBar::LightContainer(m_lightData));
      }

      m_renderable = renderable;
    }

    void LightNode::applyTransformation(util::Vector<float, 3> position, util::Quaternion<float> rotation)
    {
      if(m_lightType != DIRECTIONALLIGHT) m_lightData.position = m_position + position;
      if(m_lightType != POINTLIGHT) m_lightData.direction = rotation.apply(m_direction);
    }

    void LightNode::setPosition(util::Vector<float, 3> position)
    {
      m_position = position;
    }

    util::Vector<float, 3> LightNode::getPosition() const
    {
      return m_position;
    }

    void LightNode::setDirection(util::Vector<float, 3> direction)
    {
      m_direction = direction;
    }

    util::Vector<float, 3> LightNode::getDirection() const
    {
      return m_direction;
    }

    void LightNode::setIntensity(float intensity)
    {
      m_lightData.intensity = intensity;
    }

    float LightNode::getIntensity() const
    {
      return m_lightData.intensity;
    }

    void LightNode::setSpotLightExponent(float spotLightExponent)
    {
      m_lightData.spotLightExponent = spotLightExponent;
    }

    float LightNode::getSpotLightExponent() const
    {
      return m_lightData.spotLightExponent;
    }

    void LightNode::setSpotLightCutoff(float spotLightCutoff)
    {
      m_lightData.spotLightCutoff = spotLightCutoff;
    }

    float LightNode::getSpotLightCutoff() const
    {
      return m_lightData.spotLightCutoff;
    }

    void LightNode::setConstAttenuation(float constAttenuation)
    {
      m_lightData.constAttenuation = constAttenuation;
    }

    float LightNode::getConstAttenuation() const
    {
      return m_lightData.constAttenuation;
    }

    void LightNode::setLinearAttenuation(float linearAttenuation)
    {
      m_lightData.linearAttenuation = linearAttenuation;
    }

    float LightNode::getLinearAttenuation() const
    {
      return m_lightData.linearAttenuation;
    }

    void LightNode::setQuadricAttenuation(float quadricAttenuation)
    {
      m_lightData.quadricAttenuation = quadricAttenuation;
    }

    float LightNode::getQuadricAttenuation() const
    {
      return m_lightData.quadricAttenuation;
    }
  }
}