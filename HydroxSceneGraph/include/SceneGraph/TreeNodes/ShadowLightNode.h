#ifndef SHADOWLIGHTNODE_H_
#define SHADOWLIGHTNODE_H_

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

    class GRAPHICAPI ShadowLightNode : public TreeNode
    {
    public:

      ShadowLightNode(LightType lightType, util::EventManager *eventManager, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      virtual ~ShadowLightNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      void setShadowProjection (float near, float far);

      util::Matrix<float, 4> getShadowProjectionMatrix() const;

      virtual void setRenderable(bool renderable);

      void applyTransformation(util::Vector<float, 3> position, util::Quaternion<float> rotation);

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

    private:

      util::EventManager *m_eventManager;

      LightType m_lightType;

      bool m_renderable;

      db::ShadowLight m_lightData;

      util::Matrix<float, 4> m_projectionMatrix;
    };
  }
}

#endif
