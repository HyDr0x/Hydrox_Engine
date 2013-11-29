#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"
#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"

namespace he
{
  AnimatedTransformNode::AnimatedTransformNode(const std::vector<AnimationTrack>& animationTracks, AnimatedGeoNode* animatedMesh, unsigned int boneIndex, 
    Matrix<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) 
    : TransformNode(trfMatrix, nodeName, parent, nextSibling, firstChild), m_animationTracks(animationTracks), m_animatedMesh(animatedMesh), m_boneIndex(boneIndex), m_currentTrack(0), m_currentAnimationTime(0.0f)
  {
    m_animatedTranslation = m_translation;
    m_animatedRotation = m_rotation;
    m_animatedScale = m_scale;
  }

  AnimatedTransformNode::AnimatedTransformNode(const std::vector<AnimationTrack>& animationTracks, AnimatedGeoNode* animatedMesh, unsigned int boneIndex, 
    Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild)
    : TransformNode(translation, scale, rotation, nodeName, parent, nextSibling, firstChild), m_animationTracks(animationTracks), m_animatedMesh(animatedMesh), m_boneIndex(boneIndex), m_currentTrack(0), m_currentAnimationTime(0.0f)
  {
    m_animatedTranslation = m_translation;
    m_animatedRotation = m_rotation;
    m_animatedScale = m_scale;
  }

  AnimatedTransformNode& AnimatedTransformNode::operator=(const AnimatedTransformNode& sourceNode)
  {
    TransformNode::operator=(sourceNode);

    m_animatedTranslation = sourceNode.m_animatedTranslation;
    m_animatedRotation = sourceNode.m_animatedRotation;
    m_animatedScale = sourceNode.m_animatedScale;

    m_animationTracks = sourceNode.m_animationTracks;
    m_currentTrack = sourceNode.m_currentTrack;
    m_animatedMesh = sourceNode.m_animatedMesh;
    m_boneIndex = sourceNode.m_boneIndex;

    return *this;
  }

  TreeNode& AnimatedTransformNode::operator=(const TreeNode& sourceNode)
  {
    assert(typeid(*this) == typeid(sourceNode));

    const AnimatedTransformNode& copyNode = static_cast<const AnimatedTransformNode&>(sourceNode);
    AnimatedTransformNode::operator=(copyNode);

    return *this;
  }

  AnimatedTransformNode::~AnimatedTransformNode()
  {
  }

  GroupNode* AnimatedTransformNode::clone() const
  {
    AnimatedTransformNode *newNode = new AnimatedTransformNode(m_animationTracks, m_animatedMesh, m_boneIndex, Matrix<float, 4>::identity(), m_nodeName);

    newNode->m_nodeName = m_nodeName;
    newNode->m_dirtyFlag = m_dirtyFlag;

    newNode->m_translation = m_translation;
    newNode->m_scale = m_scale;
    newNode->m_rotation = m_rotation;

    newNode->m_currentTrack = m_currentTrack;

    newNode->m_animatedTranslation = m_animatedTranslation;
    newNode->m_animatedRotation = m_animatedRotation;
    newNode->m_animatedScale = m_animatedScale;

    return newNode;
  }

  bool AnimatedTransformNode::ascendTraverse(Traverser* traverser)
  {
    return traverser->ascendTraverse(this);
  }

  bool AnimatedTransformNode::preTraverse(Traverser* traverser)
  {
    return traverser->preTraverse(this);
  }

  void AnimatedTransformNode::postTraverse(Traverser* traverser)
  {
    traverser->postTraverse(this);
  }

  void AnimatedTransformNode::setCurrentAnimationTrack(unsigned int currentTrack)
  {
    assert(currentTrack < m_animationTracks.size() && " Animation Track too big ");

    m_currentTrack = currentTrack;
  }

  unsigned int AnimatedTransformNode::getCurrentAnimationTrack()
  {
    return m_currentTrack;
  }

  void AnimatedTransformNode::setCurrentTime(float time)
  {
    m_currentAnimationTime = time;
  }

  void AnimatedTransformNode::addCurrentTime(float time)
  {
    m_currentAnimationTime += time;
  }

  void AnimatedTransformNode::calculateTransformation(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation)
  {
    AnimationTrack& currentTrack = m_animationTracks[m_currentTrack];

    Vector<float, 3> animatedTranslation;
    Quaternion<float> animatedRotation;
    float animatedScale;

    for(unsigned int i = 0; i < currentTrack.m_scalesTime.size(); i++)
    {
      if(currentTrack.m_scalesTime[i] > m_currentAnimationTime && i > 0)
      {
        animatedScale = currentTrack.m_scales[i - 1] + m_currentAnimationTime / currentTrack.m_scalesTime[i] * (currentTrack.m_scales[i] - currentTrack.m_scales[i - 1]);
        break;
      }
    }

    for(unsigned int i = 0; i < currentTrack.m_rotationsTime.size(); i++)
    {
      if(currentTrack.m_rotationsTime[i] > m_currentAnimationTime && i > 0)
      {
        animatedRotation = Quaternion<float>::slerp(currentTrack.m_rotations[i - 1], currentTrack.m_rotations[i], m_currentAnimationTime / currentTrack.m_rotationsTime[i]);
        break;
      }
    }

    for(unsigned int i = 0; i < currentTrack.m_positionsTime.size(); i++)
    {
      if(currentTrack.m_positionsTime[i] > m_currentAnimationTime && i > 0)
      {
        animatedTranslation = Vector<float, 3>::lerp(currentTrack.m_positions[i - 1], currentTrack.m_positions[i], m_currentAnimationTime / currentTrack.m_positionsTime[i]);
        break;
      }
    }

    m_animatedTranslation += animatedRotation.apply(m_translation * animatedScale);
    m_animatedRotation = animatedRotation * m_rotation;
    m_animatedScale = animatedScale * m_scale;

    translation += rotation.apply(m_animatedTranslation * scale);
    rotation *= m_animatedRotation;
    scale *= m_animatedScale;

    if(m_animatedMesh != nullptr)
    {
      m_animatedMesh->setBoneTransform(math::createTransformationMatrix(translation, scale, rotation), m_boneIndex);
    }
  }

  Vector<float, 3> AnimatedTransformNode::getPosition()
  {
    return m_animatedTranslation;
  }

  Quaternion<float> AnimatedTransformNode::getRotation()
  {
    return m_animatedRotation;
  }

  float AnimatedTransformNode::getScale()
  {
    return m_animatedScale;
  }
}