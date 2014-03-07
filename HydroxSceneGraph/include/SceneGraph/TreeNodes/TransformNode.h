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

      TransformNode(util::Matrix<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
      TransformNode(util::Vector<float, 3>& translation, float scale, util::Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
      TransformNode& operator=(const TransformNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);
      virtual ~TransformNode();

      virtual GroupNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool isTransformNode();

      virtual void calculateTransformation(util::Vector<float, 3>& translation, float& scale, util::Quaternion<float>& rotation);

      virtual util::Vector<float, 3> getGlobalPosition();
      virtual util::Quaternion<float> getGlobalRotation();
	    virtual float getGlobalScale();

      virtual util::Vector<float, 3> getLocalPosition();
	    virtual util::Quaternion<float> getLocalRotation();
	    virtual float getLocalScale();

      void setGlobalTranslation(float x, float y, float z);
      void setGlobalTranslation(util::Vector<float, 3> v);

      void setGlobalRotation(util::Quaternion<float> q);

      void setGlobalScale(float s);

      void setLocalTranslation(float x, float y, float z);
	    void setLocalTranslation(util::Vector<float, 3> v);

	    void addLocalTranslation(float x, float y, float z);
	    void addLocalTranslation(util::Vector<float, 3> v);

      void setLocalRotation(util::Quaternion<float> q);
      void addLocalRotation(util::Quaternion<float> q);

	    void setLocalRotationX(float angle);
	    void addLocalRotationX(float angle);
	    void setLocalRotationY(float angle);
	    void addLocalRotationY(float angle);
	    void setLocalRotationZ(float angle);
	    void addLocalRotationZ(float angle);
	    void setLocalRotationXYZ(util::Vector<float, 3> angle);
	    void addLocalRotationXYZ(util::Vector<float, 3> angle);
      void setLocalRotationAxis(float angle, util::Vector<float, 3> axis);
      void addLocalRotationAxis(float angle, util::Vector<float, 3> axis);

	    void setLocalScale(float s);
	    void addLocalScale(float s);

    protected:

      util::Quaternion<float> m_rotation;
      util::Vector<float, 3> m_translation;
      float m_scale;
    };
	}
}

#endif
