#include "SceneGraph/TreeNodes/ShadowLightNode.h"

#include <cassert>

#include <Utilities/Signals/EventManager.h>

#include <XBar/ShadowLightContainer.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    ShadowLightNode::ShadowLightNode(float near, float far, LightType lightType, util::SharedPointer<util::EventManager> eventManager, const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) :
      TreeNode(nodeName, parent, nextSibling),
      m_lightType(lightType),
      m_eventManager(eventManager),
      m_directionalNearSize(10.0f, 10.0f),
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
      m_reflectiveShadow = sourceNode.m_reflectiveShadow;
      m_lightData.light = sourceNode.m_lightData.light;
      m_directionalNearSize = sourceNode.m_directionalNearSize;
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

      newNode->m_lightData.light = m_lightData.light;

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

      float width;

      switch(m_lightType)
      {
      case SPOTLIGHT:
        width = tanf(acosf(m_lightData.light.direction[3]) * 0.5f) * 2.25f * m_near;
        m_lightData.light.projectionParameter[0] = m_near;
        m_lightData.light.projectionParameter[1] = m_far;
        m_lightData.light.projectionParameter[2] = 2.0f * width;
        m_lightData.light.projectionParameter[3] = 2.0f * width;
        m_projectionMatrix = util::math::createPerspective(-width, width, -width, width, m_near, m_far);
        break;
      case POINTLIGHT:
        break;
      case DIRECTIONALLIGHT:
        m_lightData.light.projectionParameter[0] = m_near;
        m_lightData.light.projectionParameter[1] = m_far;
        m_lightData.light.projectionParameter[2] = 2.0f * m_directionalNearSize[0];
        m_lightData.light.projectionParameter[3] = 2.0f * m_directionalNearSize[1];
        m_projectionMatrix = util::math::createOrthographic(-m_directionalNearSize[0], m_directionalNearSize[0], -m_directionalNearSize[1], m_directionalNearSize[1], m_near, m_far);
        break;
      }
    }

    void ShadowLightNode::setDirectionalLightNearSize(util::vec2f directionalNearSize)
    {
      m_directionalNearSize = directionalNearSize;
      setShadowProjection(m_near, m_far);
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
      if(m_lightType != DIRECTIONALLIGHT) m_lightData.light.position = position;
      if(m_lightType != POINTLIGHT) m_lightData.light.direction = rotation.getRotationAxis() * -1.0f;//negate it, zaxis inverted in view space and shader need the negated lightDir anyways

      m_lightData.viewProj = m_projectionMatrix * util::math::createLookAt(position, m_lightData.light.direction, util::vec3f(0.0f, 1.0f, 0.0f));
    }

    LightType ShadowLightNode::getLightType() const
    {
      return m_lightType;
    }

    void ShadowLightNode::setColor(util::vec3f color)
    {
      m_lightData.light.color = color;
    }

    util::vec3f ShadowLightNode::getColor() const
    {
      return m_lightData.light.color;
    }

    void ShadowLightNode::setLuminousFlux(float intensity)
    {
      m_lightData.light.luminousFlux = intensity;
    }

    float ShadowLightNode::getLuminousFlux() const
    {
      return m_lightData.light.luminousFlux;
    }

    void ShadowLightNode::setSpotLightExponent(float spotLightExponent)
    {
      m_lightData.light.position[3] = spotLightExponent;
    }

    float ShadowLightNode::getSpotLightExponent() const
    {
      return m_lightData.light.position[3];
    }

    void ShadowLightNode::setSpotLightCutoff(float spotLightCutoff)
    {
      m_lightData.light.direction[3] = spotLightCutoff;
      setShadowProjection(m_near, m_far);
    }

    float ShadowLightNode::getSpotLightCutoff() const
    {
      return m_lightData.light.direction[3];
    }

    void ShadowLightNode::setConstAttenuation(float constAttenuation)
    {
      m_lightData.light.constAttenuation = constAttenuation;
    }

    float ShadowLightNode::getConstAttenuation() const
    {
      return m_lightData.light.constAttenuation;
    }

    void ShadowLightNode::setLinearAttenuation(float linearAttenuation)
    {
      m_lightData.light.linearAttenuation = linearAttenuation;
    }

    float ShadowLightNode::getLinearAttenuation() const
    {
      return m_lightData.light.linearAttenuation;
    }

    void ShadowLightNode::setQuadricAttenuation(float quadricAttenuation)
    {
      m_lightData.light.quadricAttenuation = quadricAttenuation;
    }

    float ShadowLightNode::getQuadricAttenuation() const
    {
      return m_lightData.light.quadricAttenuation;
    }

    void ShadowLightNode::read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      TreeNode::read(stream, eventManager, resourceHandles);

      stream >> m_lightData.light.position;
      stream >> m_lightData.light.direction;
      stream >> m_lightData.light.color;
      stream >> m_lightData.light.luminousFlux;

      stream >> m_lightData.light.constAttenuation;
      stream >> m_lightData.light.linearAttenuation;
      stream >> m_lightData.light.quadricAttenuation;

      unsigned int type;
      stream >> type;
      m_lightType = (LightType)type;

      stream >> m_near;
      stream >> m_far;

      unsigned int fakeBool;
      stream >> fakeBool;
      m_reflectiveShadow = (bool)fakeBool;

      setShadowProjection(m_near, m_far);

      m_eventManager = eventManager;

      m_renderable = false;
    }

    void ShadowLightNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
    {
      TreeNode::write(stream, resourceHandles);

      stream << m_lightData.light.position << std::endl;
      stream << m_lightData.light.direction << std::endl;
      stream << m_lightData.light.color << std::endl;
      stream << m_lightData.light.luminousFlux << std::endl;

      stream << m_lightData.light.constAttenuation << std::endl;
      stream << m_lightData.light.linearAttenuation << std::endl;
      stream << m_lightData.light.quadricAttenuation << std::endl;

      stream << (unsigned int)m_lightType << std::endl;

      stream << m_near << std::endl;
      stream << m_far << std::endl;

      stream << m_reflectiveShadow << std::endl;
    }
  }
}