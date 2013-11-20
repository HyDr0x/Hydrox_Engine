#ifndef TRANSFORMNODE_H_
#define TRANSFORMNODE_H_

#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Utility/Observer/Subject.hpp"
#include "Hydrox/Utility/Tree/GroupNode.h"

class Traverser;

class GRAPHICAPI TransformNode : public GroupNode, public Subject<TransformNode*>
{
public:

  TransformNode(Matrix<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
  TransformNode(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
  TransformNode& operator=(const TransformNode& destinationNode);
  virtual TreeNode& operator=(const TreeNode& destinationNode);
  virtual ~TransformNode();

  virtual GroupNode* clone() const;

  virtual bool ascendTraverse(Traverser* traverser);
  virtual bool preTraverse(Traverser* traverser);
  virtual void postTraverse(Traverser* traverser);

  void calculateTransformation(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation);

  Vector<float, 3> getPosition();
	Quaternion<float> getRotation();
	float getScale();

  void setTranslation(float x, float y, float z);
	void setTranslation(Vector<float, 3> v);

	void addTranslation(float x, float y, float z);
	void addTranslation(Vector<float, 3> v);

  void setRotation(Quaternion<float> q);
  void addRotation(Quaternion<float> q);

	void setRotationX(float angle);
	void addRotationX(float angle);
	void setRotationY(float angle);
	void addRotationY(float angle);
	void setRotationZ(float angle);
	void addRotationZ(float angle);
	void setRotationXYZ(Vector<float, 3> angle);
	void addRotationXYZ(Vector<float, 3> angle);
  void setRotationAxis(float angle, Vector<float, 3> axis);
  void addRotationAxis(float angle, Vector<float, 3> axis);

	void setScale(float s);
	void addScale(float s);

private:

  Quaternion<float> m_rotation;
  Vector<float, 3> m_translation;
  float m_scale;
};

#endif
