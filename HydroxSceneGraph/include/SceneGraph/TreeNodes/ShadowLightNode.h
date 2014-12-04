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

      ShadowLightNode(){}
      ShadowLightNode(float near, float far, LightType lightType, util::SharedPointer<util::EventManager> eventManager, const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0);
      ShadowLightNode(const ShadowLightNode& sourceNode);
      ShadowLightNode(const TreeNode& sourceNode);
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

      void setReflectiveShadow(bool reflectiveShadow);

      void applyTransformation(util::vec3f position, util::Quaternion<float> rotation);

      LightType getLightType() const;

      void setColor(util::vec3f color);
      util::vec3f getColor() const;

      void setLuminousFlux(float intensity);
      float getLuminousFlux() const;

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

      virtual void read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const;

    private:

      db::ShadowLight m_lightData;

      util::Matrix<float, 4> m_projectionMatrix;
      float m_near;
      float m_far;

      util::SharedPointer<util::EventManager> m_eventManager;

      LightType m_lightType;

      bool m_renderable;
      bool m_reflectiveShadow;
    };
  }
}

#endif
