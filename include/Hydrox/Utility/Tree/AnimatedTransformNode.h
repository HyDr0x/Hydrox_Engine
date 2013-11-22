#ifndef ANIMATEDTRANSFORMNODE_H_
#define ANIMATEDTRANSFORMNODE_H_

#include <map>
#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"
#include "Hydrox/Utility/Tree/TransformNode.h"

class Traverser;
class AnimatedGeoNode;

struct AnimationTrack//an animation track, each track represents all the keys for one animation in one node
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

class GRAPHICAPI AnimatedTransformNode : public TransformNode
{
public:

  AnimatedTransformNode(Matrix<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
  AnimatedTransformNode(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
  AnimatedTransformNode& operator=(const AnimatedTransformNode& sourceNode);
  virtual TreeNode& operator=(const TreeNode& sourceNode);
  virtual ~AnimatedTransformNode();

  virtual GroupNode* clone() const;

  virtual bool ascendTraverse(Traverser* traverser);
  virtual bool preTraverse(Traverser* traverser);
  virtual void postTraverse(Traverser* traverser);

private:

  std::map<std::string, AnimationTrack> m_animationTracks;//tha animation tracks for skeletal and skinned animation
  unsigned int m_boneIndex;//index into the bone vector of the animatedGeoNode
  AnimatedGeoNode* m_animatedMesh;//the corresponding animatedGeoNode
};

#endif
