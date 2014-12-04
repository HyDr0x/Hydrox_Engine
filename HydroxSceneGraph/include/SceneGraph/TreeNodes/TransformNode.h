#ifndef TRANSFORMNODE_H_
#define TRANSFORMNODE_H_

#include <vector>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Observer/Subject.hpp>
#include <Utilities/Math/Math.hpp>
#include "SceneGraph/TreeNodes/GroupNode.h"

namespace he
{
  namespace sg
  {
    class Traverser;

    class GRAPHICAPI TransformNode : public GroupNode, public util::Subject<TransformNode*>
    {
    public:

      TransformNode(){}
      TransformNode(util::Matrix<float, 4>& trfMatrix, const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0, NodeIndex firstChild = ~0);
      TransformNode(util::vec3f& translation, float scale, util::Quaternion<float>& rotation, const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0, NodeIndex firstChild = ~0);
      TransformNode(const TransformNode& sourceNode);
      TransformNode(const TreeNode& sourceNode);
      virtual ~TransformNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual GroupNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      virtual void calculateTransformation(util::vec3f& translation, float& scale, util::Quaternion<float>& rotation) const;

      //virtual util::vec3f getGlobalPosition();
      //virtual util::Quaternion<float> getGlobalRotation();
      //virtual float getGlobalScale();

      virtual util::vec3f getLocalPosition() const;
      virtual util::Quaternion<float> getLocalRotation() const;
      virtual float getLocalScale() const;

      //void setGlobalTranslation(float x, float y, float z);
      //void setGlobalTranslation(const util::vec3f& v);

      //void setGlobalRotation(const util::Quaternion<float>& q);

      //void setGlobalScale(float s);

      void setLocalTranslation(float x, float y, float z);
      void setLocalTranslation(const util::vec3f& v);

      void addLocalTranslation(float x, float y, float z);
      void addLocalTranslation(const util::vec3f& v);

      void setLocalRotation(const util::Quaternion<float>& q);
      void addLocalRotation(const util::Quaternion<float>& q);

      void setLocalRotationX(float angle);
      void addLocalRotationX(float angle);
      void setLocalRotationY(float angle);
      void addLocalRotationY(float angle);
      void setLocalRotationZ(float angle);
      void addLocalRotationZ(float angle);
      void setLocalRotationXYZ(const util::vec3f& angle);
      void addLocalRotationXYZ(const util::vec3f& angle);
      void setLocalRotationAxis(float angle, const util::vec3f& axis);
      void addLocalRotationAxis(float angle, const util::vec3f& axis);

      void setLocalScale(float s);
      void addLocalScale(float s);

      virtual void read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const;

    protected:

      util::Quaternion<float> m_rotation;
      util::vec3f m_translation;
      float m_scale;
    };
  }
}

#endif
