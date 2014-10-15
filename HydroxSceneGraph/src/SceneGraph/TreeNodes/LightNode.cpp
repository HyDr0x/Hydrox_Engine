#include "SceneGraph/TreeNodes/LightNode.h"

#include <assert.h>

#include <Utilities/Signals/EventManager.h>

#include <XBar/LightContainer.h>

#include <DataBase/Light.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    LightNode::LightNode(LightType lightType, util::EventManager *eventManager, const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) :
      TreeNode(nodeName, parent, nextSibling), 
      m_eventManager(eventManager),
      m_lightType(lightType),
      m_renderable(false)
    {
      m_index.nodeType = LIGHTNODE;
    }

    LightNode::LightNode(const LightNode& sourceNode) : TreeNode(sourceNode)
    {
      m_eventManager = sourceNode.m_eventManager;
      m_lightType = sourceNode.m_lightType;
      m_renderable = sourceNode.m_renderable;
      m_lightData = sourceNode.m_lightData;
    }

    LightNode::LightNode(const TreeNode& sourceNode) : TreeNode(sourceNode)
    {
      assert(LIGHTNODE == sourceNode.getNodeType());

      const LightNode& copyNode = static_cast<const LightNode&>(sourceNode);

      new (this) LightNode(copyNode);
    }

    LightNode::~LightNode()
    {
    }

    TreeNode& LightNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~LightNode();

      new (this) LightNode(sourceNode);

      return *this;
    }

    TreeNode* LightNode::clone() const
    {
      LightNode *newNode = new LightNode(m_lightType, m_eventManager, m_nodeName);

      newNode->m_nodeName = m_nodeName;

      newNode->m_lightData = m_lightData;

      return newNode;
    }

    bool LightNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    bool LightNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void LightNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool LightNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool LightNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void LightNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
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

    void LightNode::applyTransformation(util::vec3f position, util::Quaternion<float> rotation)
    {
      if(m_lightType != DIRECTIONALLIGHT) m_lightData.position = position;
      if(m_lightType != POINTLIGHT) m_lightData.direction = rotation.getRotationAxis();
    }

    void LightNode::setLightType(LightType lightType)
    {
      m_lightType = lightType;
    }

    LightType LightNode::getLightType() const
    {
      return m_lightType;
    }

    void LightNode::setColor(util::vec3f color)
    {
      m_lightData.color = color;
    }

    util::vec3f LightNode::getColor() const
    {
      return m_lightData.color;
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
      m_lightData.position[3] = spotLightExponent;
    }

    float LightNode::getSpotLightExponent() const
    {
      return m_lightData.position[3];
    }

    void LightNode::setSpotLightCutoff(float spotLightCutoff)
    {
      m_lightData.direction[3] = spotLightCutoff;
    }

    float LightNode::getSpotLightCutoff() const
    {
      return m_lightData.direction[3];
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

    void LightNode::read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
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

      m_eventManager = eventManager;

      m_renderable = false;
    }

    void LightNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
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
    }
  }
}