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

      virtual util::Vector<float, 3> getPosition();
	    virtual util::Quaternion<float> getRotation();
	    virtual float getScale();

      void setTranslation(float x, float y, float z);
	    void setTranslation(util::Vector<float, 3> v);

	    void addTranslation(float x, float y, float z);
	    void addTranslation(util::Vector<float, 3> v);

      void setRotation(util::Quaternion<float> q);
      void addRotation(util::Quaternion<float> q);

	    void setRotationX(float angle);
	    void addRotationX(float angle);
	    void setRotationY(float angle);
	    void addRotationY(float angle);
	    void setRotationZ(float angle);
	    void addRotationZ(float angle);
	    void setRotationXYZ(util::Vector<float, 3> angle);
	    void addRotationXYZ(util::Vector<float, 3> angle);
      void setRotationAxis(float angle, util::Vector<float, 3> axis);
      void addRotationAxis(float angle, util::Vector<float, 3> axis);

	    void setScale(float s);
	    void addScale(float s);

    protected:

      util::Quaternion<float> m_rotation;
      util::Vector<float, 3> m_translation;
      float m_scale;
    };
	}
}

#endif
