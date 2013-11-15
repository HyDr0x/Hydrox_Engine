#ifndef ANIMATEDTRANSFORMNODE_H_
#define ANIMATEDTRANSFORMNODE_H_

#include <map>
#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Utility/Observer/Subject.hpp"
#include "Hydrox/Utility/Tree/GroupNode.h"

class Traverser;

struct AnimationTrack
{
  std::string m_animationName;
  std::string m_nodeName;

  std::vector<Vector<float, 3>>  m_positions;
  std::vector<float>  m_positionsTime;

  std::vector<Quaternion<float>>  m_rotations;
  std::vector<float>  m_rotationsTime;

  std::vector<Vector<float, 3>>  m_scales;
  std::vector<float>  m_scalesTime;
};

class GRAPHICAPI AnimatedTransformNode : public GroupNode, public Subject<AnimatedTransformNode*>
{
public:

  AnimatedTransformNode(Matrix<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
  AnimatedTransformNode(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
  AnimatedTransformNode& operator=(const AnimatedTransformNode& destinationNode);
  virtual TreeNode& operator=(const TreeNode& destinationNode);
  virtual ~AnimatedTransformNode();

  virtual GroupNode* clone() const;

  virtual bool ascendTraverse(Traverser* traverser);
  virtual bool preTraverse(Traverser* traverser);
  virtual void postTraverse(Traverser* traverser);

  void calculateTransformation(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation);

  Vector<float, 3> getPosition();
	Quaternion<float> getRotation();
	float getScale();

  void setPosition(float x, float y, float z);
	void setPosition(Vector<float, 3> v);

	void setTranslation(float x, float y, float z);
	void setTranslation(Vector<float, 3> v);

  void setRotation(Quaternion<float> q);
  void turnRotation(Quaternion<float> q);

	void setRotationX(float angle);
	void setTurnX(float angle);
	void setRotationY(float angle);
	void setTurnY(float angle);
	void setRotationZ(float angle);
	void setTurnZ(float angle);
	void setRotationXYZ(Vector<float, 3> angle);
	void setTurnXYZ(Vector<float, 3> angle);
  void setRotationAxis(float angle, Vector<float, 3> axis);
  void setTurnAxis(float angle, Vector<float, 3> axis);

	void setScale(float s);
	void Scale(float s);

private:

  Quaternion<float> m_rotation;
  Vector<float, 3> m_translation;
  float m_scale;

  std::map<std::string, AnimationTrack> m_animationTracks;
};

#endif
