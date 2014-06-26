#include "SceneGraph/TreeNodes/ShadowLightNode.h"

#include <assert.h>

#include <Utilities/Signals/EventManager.h>

#include <XBar/ShadowLightContainer.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    ShadowLightNode::ShadowLightNode(LightType lightType, util::EventManager *eventManager, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) :
      TreeNode(nodeName, parent, nextSibling),
      m_lightType(lightType),
      m_eventManager(eventManager),
      m_renderable(false)
    {
      setShadowProjection(1.0f, 1000.0f);
    }

    ShadowLightNode::~ShadowLightNode()
    {
    }

    TreeNode& ShadowLightNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const ShadowLightNode& copyNode = static_cast<const ShadowLightNode&>(sourceNode);
      ShadowLightNode::operator=(copyNode);

      return *this;
    }

    TreeNode* ShadowLightNode::clone() const
    {
      ShadowLightNode *newNode = new ShadowLightNode(m_lightType, m_eventManager, m_nodeName);

      newNode->m_lightData = m_lightData;

      return newNode;
    }

    bool ShadowLightNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(this);
    }

    bool ShadowLightNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void ShadowLightNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool ShadowLightNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(this);
    }

    bool ShadowLightNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void ShadowLightNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    bool ShadowLightNode::isShadowLightNode() const
    {
      return true;
    }

    void ShadowLightNode::setShadowProjection(float near, float far)
    {
      switch(m_lightType)
      {
      case SPOTLIGHT:
        m_projectionMatrix = util::math::createPerspective(45.0f, 1.0f, near, far);
        break;
      case POINTLIGHT:
        break;
      case DIRECTIONALLIGHT:
        m_projectionMatrix = util::math::createOrthographic(-1.0f, 1.0f, -1.0f, 1.0f, near, far);
        break;
      }
    }

    util::Matrix<float, 4> ShadowLightNode::getShadowProjectionMatrix() const
    {
      return m_projectionMatrix;
    }

    void ShadowLightNode::setRenderable(bool renderable)
    {
      if(!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnAddShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData));
      }
      else if (m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnRemoveShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData));
      }

      m_renderable = renderable;
    }

    void ShadowLightNode::applyTransformation(util::Vector<float, 3> position, util::Quaternion<float> rotation)
    {
      if(m_lightType != DIRECTIONALLIGHT) m_lightData.position = position;
      if(m_lightType != POINTLIGHT) m_lightData.direction = rotation.getRotationAxis();

      m_lightData.viewProj = m_projectionMatrix * util::math::createLookAt(m_lightData.position, m_lightData.direction, util::Vector<float, 3>(0.0f, 1.0f, 0.0f));
    }

    void ShadowLightNode::setColor(util::Vector<float, 3> color)
    {
      m_lightData.color = color;
    }

    util::Vector<float, 3> ShadowLightNode::getColor() const
    {
      return m_lightData.color;
    }

    void ShadowLightNode::setIntensity(float intensity)
    {
      m_lightData.intensity = intensity;
    }

    float ShadowLightNode::getIntensity() const
    {
      return m_lightData.intensity;
    }

    void ShadowLightNode::setSpotLightExponent(float spotLightExponent)
    {
      m_lightData.position[3] = spotLightExponent;
    }

    float ShadowLightNode::getSpotLightExponent() const
    {
      return m_lightData.position[3];
    }

    void ShadowLightNode::setSpotLightCutoff(float spotLightCutoff)
    {
      m_lightData.direction[3] = spotLightCutoff;
    }

    float ShadowLightNode::getSpotLightCutoff() const
    {
      return m_lightData.direction[3];
    }

    void ShadowLightNode::setConstAttenuation(float constAttenuation)
    {
      m_lightData.constAttenuation = constAttenuation;
    }

    float ShadowLightNode::getConstAttenuation() const
    {
      return m_lightData.constAttenuation;
    }

    void ShadowLightNode::setLinearAttenuation(float linearAttenuation)
    {
      m_lightData.linearAttenuation = linearAttenuation;
    }

    float ShadowLightNode::getLinearAttenuation() const
    {
      return m_lightData.linearAttenuation;
    }

    void ShadowLightNode::setQuadricAttenuation(float quadricAttenuation)
    {
      m_lightData.quadricAttenuation = quadricAttenuation;
    }

    float ShadowLightNode::getQuadricAttenuation() const
    {
      return m_lightData.quadricAttenuation;
    }
  }
}