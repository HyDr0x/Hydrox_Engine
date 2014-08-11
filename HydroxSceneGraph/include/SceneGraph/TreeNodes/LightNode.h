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

    class GRAPHICAPI LightNode : public TreeNode
    {
    public:

      LightNode(){}
      LightNode(LightType lightType, util::EventManager *eventManager, const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0);
      LightNode(const LightNode& sourceNode);
      LightNode(const TreeNode& sourceNode);
      virtual ~LightNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      virtual void setRenderable(bool renderable);

      void applyTransformation(util::Vector<float, 3> position, util::Quaternion<float> rotation);

      void setLightType(LightType lightType);
      LightType getLightType() const;

      void setColor(util::Vector<float, 3> color);
      util::Vector<float, 3> getColor() const;

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

    protected:

      util::EventManager *m_eventManager;

      LightType m_lightType;

      bool m_renderable;

      db::Light m_lightData;
    };
  }
}

#endif
