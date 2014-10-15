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
    ShadowLightNode::ShadowLightNode(float near, float far, LightType lightType, util::EventManager *eventManager, const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) :
      TreeNode(nodeName, parent, nextSibling),
      m_lightType(lightType),
      m_eventManager(eventManager),
      m_renderable(false),
      m_reflectiveShadow(false)
    {
      setShadowProjection(near, far);

      m_index.nodeType = SHADOWLIGHTNODE;
    }

    ShadowLightNode::ShadowLightNode(const ShadowLightNode& sourceNode) : TreeNode(sourceNode)
    {
      m_eventManager = sourceNode.m_eventManager;
      m_lightType = sourceNode.m_lightType;
      m_renderable = sourceNode.m_renderable;
      m_lightData = sourceNode.m_lightData;
      m_near = sourceNode.m_near;
      m_far = sourceNode.m_far;
      m_projectionMatrix = sourceNode.m_projectionMatrix;
    }

    ShadowLightNode::ShadowLightNode(const TreeNode& sourceNode) : TreeNode(sourceNode)
    {
      assert(SHADOWLIGHTNODE == sourceNode.getNodeType());

      const ShadowLightNode& copyNode = static_cast<const ShadowLightNode&>(sourceNode);

      new (this) ShadowLightNode(copyNode);
    }

    ShadowLightNode::~ShadowLightNode()
    {
    }

    TreeNode& ShadowLightNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~ShadowLightNode();

      new (this) ShadowLightNode(sourceNode);

      return *this;
    }

    TreeNode* ShadowLightNode::clone() const
    {
      ShadowLightNode *newNode = new ShadowLightNode(m_near, m_far, m_lightType, m_eventManager, m_nodeName);

      newNode->m_lightData = m_lightData;

      return newNode;
    }

    bool ShadowLightNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    bool ShadowLightNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void ShadowLightNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool ShadowLightNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool ShadowLightNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void ShadowLightNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
    }

    void ShadowLightNode::setShadowProjection(float near, float far)
    {
      m_near = near;
      m_far = far;

      float fov = 0.0f;

      switch(m_lightType)
      {
      case SPOTLIGHT:
        fov = util::math::radToDeg(acosf(m_lightData.direction[3]));
        m_projectionMatrix = util::math::createPerspective(fov, 1.0f, m_near, m_far);
        break;
      case POINTLIGHT:
        break;
      case DIRECTIONALLIGHT:
        m_projectionMatrix = util::math::createOrthographic(-100.0f, 100.0f, -100.0f, 100.0f, m_near, m_far);
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
        if(m_reflectiveShadow)
        {
          m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnAddReflectiveShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData, m_reflectiveShadow));
        }
        else
        {
          m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnAddShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData, m_reflectiveShadow));
        }
      }
      else if (m_renderable && !renderable)
      {
        if(m_reflectiveShadow)
        {
          m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnRemoveReflectiveShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData, m_reflectiveShadow));
        }
        else
        {
          m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnRemoveShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData, m_reflectiveShadow));
        }
      }

      m_renderable = renderable;
    }

    void ShadowLightNode::setReflectiveShadow(bool reflectiveShadow)
    {
      if(m_renderable)
      {
        if(m_reflectiveShadow)
        {
          m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnRemoveReflectiveShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData, m_reflectiveShadow));
        }
        else
        {
          m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnRemoveShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData, m_reflectiveShadow));
        }
      }

      m_reflectiveShadow = reflectiveShadow;

      if(m_renderable)
      {
        if(m_reflectiveShadow)
        {
          m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnAddReflectiveShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData, m_reflectiveShadow));
        }
        else
        {
          m_eventManager->raiseSignal<void(*)(const xBar::ShadowLightContainer& light)>(util::EventManager::OnAddShadowLightNode)->execute(xBar::ShadowLightContainer(m_lightData, m_reflectiveShadow));
        }
      }
    }

    void ShadowLightNode::applyTransformation(util::vec3f position, util::Quaternion<float> rotation)
    {
      if(m_lightType != DIRECTIONALLIGHT) m_lightData.position = position;
      if(m_lightType != POINTLIGHT) m_lightData.direction = rotation.getRotationAxis() * -1.0f;

      m_lightData.viewProj = m_projectionMatrix * util::math::createLookAt(position, m_lightData.direction, util::vec3f(0.0f, 1.0f, 0.0f));
    }

    LightType ShadowLightNode::getLightType() const
    {
      return m_lightType;
    }

    void ShadowLightNode::setColor(util::vec3f color)
    {
      m_lightData.color = color;
    }

    util::vec3f ShadowLightNode::getColor() const
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
      setShadowProjection(m_near, m_far);
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

    void ShadowLightNode::read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      TreeNode::read(stream, eventManager, resourceHandles);

      stream >> m_lightData.position;
      stream >> m_lightData.direction;
      stream >> m_lightData.color;
      stream >> m_lightData.intensity;

      stream >> m_lightData.constAttenuation;
      stream >> m_lightData.linearAttenuation;
      stream >> m_lightData.quadricAttenuation;

      unsigned int type;
      stream >> type;
      m_lightType = (LightType)type;

      stream >> m_near;
      stream >> m_far;

      stream >> m_reflectiveShadow;

      setShadowProjection(m_near, m_far);

      m_eventManager = eventManager;

      m_renderable = false;
    }

    void ShadowLightNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
    {
      TreeNode::write(stream, resourceHandles);

      stream << m_lightData.position << std::endl;
      stream << m_lightData.direction << std::endl;
      stream << m_lightData.color << std::endl;
      stream << m_lightData.intensity << std::endl;

      stream << m_lightData.constAttenuation << std::endl;
      stream << m_lightData.linearAttenuation << std::endl;
      stream << m_lightData.quadricAttenuation << std::endl;

      stream << (unsigned int)m_lightType << std::endl;

      stream << m_near << std::endl;
      stream << m_far << std::endl;

      stream << m_reflectiveShadow;
    }
  }
}