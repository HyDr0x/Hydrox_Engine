#ifndef LIGHTNODE_H_
#define LIGHTNODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>

#include <DataBase/Light.h>

#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
  namespace util
  {
    class EventManager;
  }

  namespace sg
  {
    class Traverser;

    enum LightType
    {
      POINTLIGHT,
      DIRECTIONALLIGHT,
      SPOTLIGHT,
    };

    class GRAPHICAPI LightNode : public TreeNode
    {
    public:

      LightNode(LightType lightType, util::EventManager *eventManager, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      virtual ~LightNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      virtual bool isLightNode() const;

      void setRenderable(bool renderable);

      void applyTransformation(util::Vector<float, 3> position, util::Quaternion<float> rotation);

      void setPosition(util::Vector<float, 3> position);
      util::Vector<float, 3> getPosition() const;

      void setDirection(util::Vector<float, 3> direction);
      util::Vector<float, 3> getDirection() const;

      void setIntensity(float intensity);
      float getIntensity() const;

      void setSpotLightExponent(float spotLightExponent);
      float getSpotLightExponent() const;

      void setSpotLightCutoff(float spotLightCutoff);
      float getSpotLightCutoff() const;

      void setConstAttenuation(float constAttenuation);
      float getConstAttenuation() const;

      void setLinearAttenuation(float linearAttenuation);
      float getLinearAttenuation() const;

      void setQuadricAttenuation(float quadricAttenuation);
      float getQuadricAttenuation() const;

    private:

      util::EventManager *m_eventManager;

      util::Vector<float, 3> m_position;
      util::Vector<float, 3> m_direction;

      db::Light m_lightData;

      LightType m_lightType;

      bool m_renderable;
    };
  }
}

#endif
