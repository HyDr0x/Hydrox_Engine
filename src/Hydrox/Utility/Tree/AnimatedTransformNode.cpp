#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"
#include "Hydrox/Utility/Traverser/AnimationControlTraverser.h"
#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"

namespace he
{
  AnimatedTransformNode::AnimatedTransformNode(const std::vector<AnimationTrack>& animationTracks, const std::string& nodeName, 
    GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) 
    : TransformNode(Matrix<float, 4>::identity(), nodeName, parent, nextSibling, firstChild), m_animationTracks(animationTracks), m_animatedMesh(nullptr), m_boneIndex(~0), m_currentTrack(0), m_currentAnimationTime(0.0f), m_pauseAnimation(false)
  {
    m_animatedTranslation = Vector<float, 3>::identity();
    m_animatedRotation = Quaternion<float>::identity();
    m_animatedScale = 1.0f;
  }

  AnimatedTransformNode& AnimatedTransformNode::operator=(const AnimatedTransformNode& sourceNode)
  {
    TransformNode::operator=(sourceNode);

    m_animatedTranslation = sourceNode.m_animatedTranslation;
    m_animatedRotation = sourceNode.m_animatedRotation;
    m_animatedScale = sourceNode.m_animatedScale;

    m_animationTracks = sourceNode.m_animationTracks;
    m_currentTrack = sourceNode.m_currentTrack;
    m_currentAnimationTime = sourceNode.m_currentAnimationTime;

    m_animatedMesh = sourceNode.m_animatedMesh;
    m_boneIndex = sourceNode.m_boneIndex;

    m_pauseAnimation = sourceNode.m_pauseAnimation;

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
    AnimatedTransformNode *newNode = new AnimatedTransformNode(m_animationTracks, m_nodeName);

    newNode->m_dirtyFlag = m_dirtyFlag;

    newNode->m_translation = m_translation;
    newNode->m_scale = m_scale;
    newNode->m_rotation = m_rotation;

    newNode->m_animatedTranslation = m_animatedTranslation;
    newNode->m_animatedRotation = m_animatedRotation;
    newNode->m_animatedScale = m_animatedScale;

    newNode->m_currentTrack = m_currentTrack;
    newNode->m_currentAnimationTime = m_currentAnimationTime;
    newNode->m_pauseAnimation = m_pauseAnimation;

    newNode->m_animatedMesh = m_animatedMesh;
    newNode->m_boneIndex = m_boneIndex;

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

  void AnimatedTransformNode::setBoneIndex(unsigned int boneIndex)
  {
    m_boneIndex = boneIndex;
  }

  unsigned int AnimatedTransformNode::getBoneIndex()
  {
    return m_boneIndex;
  }

  void AnimatedTransformNode::setSkinnedMesh(AnimatedGeoNode* animatedMesh)
  {
    m_animatedMesh = animatedMesh;
  }

  AnimatedGeoNode* AnimatedTransformNode::getSkinnedMesh()
  {
    return m_animatedMesh;
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

  void AnimatedTransformNode::setCurrentAnimationTime(float time)
  {
    if(!m_pauseAnimation)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_currentAnimationTime = time;
    }
  }

  void AnimatedTransformNode::addCurrentAnimationTime(float time)
  {
    if(!m_pauseAnimation)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_currentAnimationTime += time;
    }
  }

  void AnimatedTransformNode::setPauseAnimation(bool pauseAnimation)
  {
    m_pauseAnimation = pauseAnimation;
  }

  bool AnimatedTransformNode::getPauseAnimation()
  {
    return m_pauseAnimation;
  }

  void AnimatedTransformNode::stopAnimation()
  {
    if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
    {
      notify(this);
      addDirtyFlag(TRF_DIRTY);
    }

    m_currentAnimationTime = 0.0f;
  }

  void AnimatedTransformNode::calculateTransformation(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation)
  {
    AnimationTrack& currentTrack = m_animationTracks[m_currentTrack];

    if(m_dirtyFlag & GroupNode::TRF_DIRTY)//update animations only if they changed
    {
      interpolateKeyFrames(currentTrack);
    }

    m_animatedTranslation += m_animatedRotation.apply(m_translation * m_animatedScale);
    m_animatedRotation *= m_rotation;
    m_animatedScale *= m_scale;

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

  void AnimatedTransformNode::setAnimatedSceneTime(AnimatedTransformNode *node, float animationTime)
  {
    AnimationControlTraverser traverser;
    traverser.setAnimationTime(animationTime);
    traverser.doTraverse(node);
  }

  void addAnimatedSceneTime(AnimatedTransformNode *node, float animationTime)
  {
    AnimationControlTraverser traverser;
    traverser.addAnimationTime(animationTime);
    traverser.doTraverse(node);
  }

  void AnimatedTransformNode::pauseAnimatedScene(AnimatedTransformNode *node, bool pauseAnimation)
  {
    AnimationControlTraverser traverser;
    traverser.setPauseAnimation(pauseAnimation);
    traverser.doTraverse(node);
  }

  void AnimatedTransformNode::stopAnimatedScene(AnimatedTransformNode *node)
  {
    AnimationControlTraverser traverser;
    traverser.setStopAnimation();
    traverser.doTraverse(node);
  }

  void AnimatedTransformNode::interpolateKeyFrames(AnimationTrack& currentTrack)
  {
    float timeInTicks = fmod(m_currentAnimationTime * currentTrack.m_animationTicksPerSecond, m_animationTracks[m_currentTrack].m_duration);

    for(unsigned int i = 0; i < currentTrack.m_scalesTime.size(); i++)
    {
      if(i + 1 < currentTrack.m_scalesTime.size())
      {
        if(currentTrack.m_scalesTime[i + 1] >= timeInTicks)
        {
          float diffTime = timeInTicks - currentTrack.m_scalesTime[i];
          if(diffTime < 0.0f)
          {
            diffTime = currentTrack.m_scalesTime[i] + timeInTicks;
          }
          m_animatedScale = currentTrack.m_scales[i] + diffTime / (currentTrack.m_scalesTime[i + 1] - currentTrack.m_scalesTime[i]) * (currentTrack.m_scales[i + 1] - currentTrack.m_scales[i]);
          break;
        }
      }
      else
      {
        m_animatedScale = currentTrack.m_scales[i];
      }
    }

    for(unsigned int i = 0; i < currentTrack.m_rotationsTime.size(); i++)
    {
      if(i + 1 < currentTrack.m_rotationsTime.size())
      {
        if(currentTrack.m_rotationsTime[i + 1] >= timeInTicks)
        {
          float diffTime = timeInTicks - currentTrack.m_rotationsTime[i];
          if(diffTime < 0.0f)
          {
            diffTime = currentTrack.m_rotationsTime[i] + timeInTicks;
          }
          m_animatedRotation = Quaternion<float>::slerp(currentTrack.m_rotations[i], currentTrack.m_rotations[i + 1], diffTime / (currentTrack.m_rotationsTime[i + 1] - currentTrack.m_rotationsTime[i]));
          break;
        }
      }
      else
      {
        m_animatedRotation = currentTrack.m_rotations[i];
      }
    }

    for(unsigned int i = 0; i < currentTrack.m_positionsTime.size(); i++)
    {
      if(i + 1 < currentTrack.m_positionsTime.size())
      {
        if(currentTrack.m_positionsTime[i + 1] >= timeInTicks)
        {
          float diffTime = timeInTicks - currentTrack.m_positionsTime[i];
          if(diffTime < 0.0f)
          {
            diffTime = currentTrack.m_positionsTime[i] + timeInTicks;
          }
          m_animatedTranslation = Vector<float, 3>::lerp(currentTrack.m_positions[i], currentTrack.m_positions[i + 1], diffTime / (currentTrack.m_positionsTime[i + 1] - currentTrack.m_positionsTime[i]));
          break;
        }
      }
      else
      {
        m_animatedTranslation = currentTrack.m_positions[i];
      }
    }
  }
}