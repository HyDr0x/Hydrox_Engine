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

  TransformNode(Mat<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
  TransformNode(Vec<float, 3>& translation, Vec<float, 3>& scale, Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
  TransformNode& operator=(const TransformNode& destinationNode);
  virtual TreeNode& operator=(const TreeNode& destinationNode);
  virtual ~TransformNode();

  virtual GroupNode* clone() const;

  virtual bool ascendTraverse(Traverser* traverser);
  virtual bool preTraverse(Traverser* traverser);
  virtual void postTraverse(Traverser* traverser);

  void calculateTransformation(Vec<float, 3>& translation, Vec<float, 3>& scale, Quaternion<float>& rotation);

  Vec<float, 3> getPosition();
	Quaternion<float> getRotation();
	Vec<float, 3> getScale();

  void setPosition(float x, float y, float z);
	void setPosition(Vec<float, 3> v);

	void setTranslation(float x, float y, float z);
	void setTranslation(Vec<float, 3> v);

	void setRotationX(float angle);
	void setTurnX(float angle);
	void setRotationY(float angle);
	void setTurnY(float angle);
	void setRotationZ(float angle);
	void setTurnZ(float angle);
	void setRotationXYZ(Vec<float, 3> angle);
	void setTurnXYZ(Vec<float, 3> angle);
  void setRotationAxis(float angle, Vec<float, 3> axis);
  void setTurnAxis(float angle, Vec<float, 3> axis);

	void setScale(float s);
	void Scale(float s);

private:

  Vec<float, 3> m_translation;
  Vec<float, 3> m_scale;
  Quaternion<float> m_rotation;
};

#endif
