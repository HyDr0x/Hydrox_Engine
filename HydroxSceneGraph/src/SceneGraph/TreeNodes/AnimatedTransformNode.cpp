#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"
#include "SceneGraph/Traverser/GetGlobalCoordinateTraverser.h"

#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"

namespace he
{
  namespace sg
  {
    AnimatedTransformNode::AnimatedTransformNode(const std::vector<AnimationTrack>& animationTracks, const std::string& nodeName, 
      NodeIndex parent, NodeIndex nextSibling, NodeIndex firstChild)
      : TransformNode(util::Matrix<float, 4>::identity(), nodeName, parent, nextSibling, firstChild), 
      m_animationTracks(animationTracks), 
      m_animatedMesh(~0), 
      m_boneIndex(~0), 
      m_currentTrack(0), 
      m_currentAnimationTimeInSeconds(0.0f), 
      m_pauseAnimation(false)
    {
      m_animatedTranslation = util::Vector<float, 3>::identity();
      m_animatedRotation = util::Quaternion<float>::identity();
      m_animatedScale = 1.0f;

      m_currentScaleKey = 0;
      m_currentPositionKey = 0;
      m_currentRotationKey = 0;

      m_index.nodeType = ANIMATEDTRANSFORMNODE;
    }

    AnimatedTransformNode::AnimatedTransformNode(const AnimatedTransformNode& sourceNode) :
      TransformNode(sourceNode)
    {
      m_animatedMesh = sourceNode.m_animatedMesh;
      m_boneIndex = sourceNode.m_boneIndex;
      m_currentTrack = sourceNode.m_currentTrack;
      m_currentAnimationTimeInSeconds = sourceNode.m_currentAnimationTimeInSeconds;
      m_pauseAnimation = sourceNode.m_pauseAnimation;

      m_animationTracks = sourceNode.m_animationTracks;

      m_animatedTranslation = sourceNode.m_animatedTranslation;
      m_animatedRotation = sourceNode.m_animatedRotation;
      m_animatedScale = sourceNode.m_animatedScale;

      m_currentScaleKey = sourceNode.m_currentScaleKey;
      m_currentPositionKey = sourceNode.m_currentPositionKey;
      m_currentRotationKey = sourceNode.m_currentRotationKey;
    }

    AnimatedTransformNode::AnimatedTransformNode(const TreeNode& sourceNode) : 
      TransformNode(sourceNode),
      m_animatedMesh(~0)
    {
      assert(ANIMATEDTRANSFORMNODE == sourceNode.getNodeType());

      const AnimatedTransformNode& copyNode = static_cast<const AnimatedTransformNode&>(sourceNode);

      new (this) AnimatedTransformNode(copyNode);
    }

    AnimatedTransformNode::~AnimatedTransformNode()
    {
    }

    TreeNode& AnimatedTransformNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~AnimatedTransformNode();

      new (this) AnimatedTransformNode(sourceNode);

      return *this;
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
      newNode->m_currentAnimationTimeInSeconds = m_currentAnimationTimeInSeconds;
      newNode->m_pauseAnimation = m_pauseAnimation;

      newNode->m_currentScaleKey = m_currentScaleKey;
      newNode->m_currentPositionKey = m_currentPositionKey;
      newNode->m_currentRotationKey = m_currentRotationKey;

      newNode->m_animatedMesh = m_animatedMesh;
      newNode->m_boneIndex = m_boneIndex;

      return newNode;
    }

    bool AnimatedTransformNode::ascendTraverse(Traverser* traverser)
    {
     return traverser->ascendTraverse(*this);
    }

    bool AnimatedTransformNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void AnimatedTransformNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool AnimatedTransformNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool AnimatedTransformNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void AnimatedTransformNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
    }

    void AnimatedTransformNode::setBoneIndex(unsigned int boneIndex)
    {
      m_boneIndex = boneIndex;
    }

    unsigned int AnimatedTransformNode::getBoneIndex() const
    {
      return m_boneIndex;
    }

    void AnimatedTransformNode::setSkinnedMesh(NodeIndex animatedMesh)
    {
      m_animatedMesh = animatedMesh;
    }

    NodeIndex AnimatedTransformNode::getSkinnedMesh() const
    {
      return m_animatedMesh;
    }

    void AnimatedTransformNode::setCurrentAnimationTrack(unsigned int currentTrack)
    {
      assert(currentTrack < m_animationTracks.size() && " Animation Track too big ");

      addDirtyFlag(ANIM_DIRTY);

      m_currentTrack = currentTrack;

      m_currentPositionKey = 0;
      m_currentRotationKey = 0;
      m_currentScaleKey = 0;
    }

    unsigned int AnimatedTransformNode::getCurrentAnimationTrack() const
    {
      return m_currentTrack;
    }

    const std::vector<AnimationTrack>& AnimatedTransformNode::getAnimationTracks() const
    {
      return m_animationTracks;
    }

    void AnimatedTransformNode::setAnimationTracks(const std::vector<AnimationTrack>& animationTracks)
    {
      m_animationTracks = animationTracks;
    }

    void AnimatedTransformNode::setCurrentAnimationTime(float time)
    {
      if(!m_pauseAnimation)
      {
        addDirtyFlag(ANIM_DIRTY);

        m_currentAnimationTimeInSeconds = time;
      }
    }

    void AnimatedTransformNode::addCurrentAnimationTime(float time)
    {
      if(!m_pauseAnimation)
      {
        addDirtyFlag(ANIM_DIRTY);

        m_currentAnimationTimeInSeconds += time;
      }
    }

    void AnimatedTransformNode::setPauseAnimation(bool pauseAnimation)
    {
      m_pauseAnimation = pauseAnimation;
    }

    bool AnimatedTransformNode::getPauseAnimation() const
    {
      return m_pauseAnimation;
    }

    void AnimatedTransformNode::stopAnimation()
    {
      addDirtyFlag(ANIM_DIRTY);

      m_pauseAnimation = true;
      m_currentAnimationTimeInSeconds = 0.0f;
    }

    void AnimatedTransformNode::calculateTransformation(util::Vector<float, 3>& translation, float& scale, util::Quaternion<float>& rotation)
    {
      AnimationTrack& currentTrack = m_animationTracks[m_currentTrack];

      if(m_dirtyFlag & GroupNode::ANIM_DIRTY && !m_pauseAnimation)//update animations only if they changed
      {
        interpolateKeyFrames(currentTrack);
      }

      translation += rotation.apply((m_animatedTranslation + m_animatedRotation.apply(m_translation * m_animatedScale)) * scale);
      rotation *= (m_animatedRotation * m_rotation);
      scale *= (m_animatedScale * m_scale);
    }

    //util::Vector<float, 3> AnimatedTransformNode::getGlobalPosition()
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  return traverser.getGlobalTranslation() + traverser.getGlobalRotation().apply(m_animatedTranslation + m_animatedRotation.apply(m_translation * m_animatedScale) * traverser.getGlobalScale());
    //}

    //util::Quaternion<float> AnimatedTransformNode::getGlobalRotation()
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  return traverser.getGlobalRotation() * m_animatedRotation * m_rotation;
    //}

    //float AnimatedTransformNode::getGlobalScale()
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  return traverser.getGlobalScale() * m_animatedScale * m_scale;
    //}

    util::Vector<float, 3> AnimatedTransformNode::getLocalPosition() const
    {
      return m_animatedTranslation + m_animatedRotation.apply(m_translation * m_animatedScale);
    }

    util::Quaternion<float> AnimatedTransformNode::getLocalRotation() const
    {
      return m_animatedRotation * m_rotation;
    }

    float AnimatedTransformNode::getLocalScale() const
    {
      return m_animatedScale * m_scale;
    }

    void AnimatedTransformNode::interpolateKeyFrames(AnimationTrack& currentTrack)
    {
      float timeInTicks = fmod(m_currentAnimationTimeInSeconds * currentTrack.m_animationTicksPerSecond, m_animationTracks[m_currentTrack].m_duration);

      if(currentTrack.m_scalesTime[m_currentScaleKey] > timeInTicks)
      {
        m_currentScaleKey = 0;
      }

      if(currentTrack.m_positionsTime[m_currentPositionKey] > timeInTicks)
      {
        m_currentPositionKey = 0;
      }

      if(currentTrack.m_rotationsTime[m_currentRotationKey] > timeInTicks)
      {
        m_currentRotationKey = 0;
      }

      for(unsigned int i = m_currentScaleKey; i < currentTrack.m_scalesTime.size(); i++)
      {
        if(i + 1 < currentTrack.m_scalesTime.size())
        {
          if(currentTrack.m_scalesTime[i + 1] >= timeInTicks)
          {
            m_currentScaleKey = i;
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

      for(unsigned int i = m_currentRotationKey; i < currentTrack.m_rotationsTime.size(); i++)
      {
        if(i + 1 < currentTrack.m_rotationsTime.size())
        {
          if(currentTrack.m_rotationsTime[i + 1] >= timeInTicks)
          {
            m_currentRotationKey = i;
            float diffTime = timeInTicks - currentTrack.m_rotationsTime[i];
            if(diffTime < 0.0f)
            {
              diffTime = currentTrack.m_rotationsTime[i] + timeInTicks;
            }
            m_animatedRotation = util::Quaternion<float>::slerp(currentTrack.m_rotations[i], currentTrack.m_rotations[i + 1], diffTime / (currentTrack.m_rotationsTime[i + 1] - currentTrack.m_rotationsTime[i]));
            break;
          }
        }
        else
        {
          m_animatedRotation = currentTrack.m_rotations[i];
        }
      }

      for(unsigned int i = m_currentPositionKey; i < currentTrack.m_positionsTime.size(); i++)
      {
        if(i + 1 < currentTrack.m_positionsTime.size())
        {
          if(currentTrack.m_positionsTime[i + 1] >= timeInTicks)
          {
            m_currentPositionKey = i;
            float diffTime = timeInTicks - currentTrack.m_positionsTime[i];
            if(diffTime < 0.0f)
            {
              diffTime = currentTrack.m_positionsTime[i] + timeInTicks;
            }
            m_animatedTranslation = util::Vector<float, 3>::lerp(currentTrack.m_positions[i], currentTrack.m_positions[i + 1], diffTime / (currentTrack.m_positionsTime[i + 1] - currentTrack.m_positionsTime[i]));
            break;
          }
        }
        else
        {
          m_animatedTranslation = currentTrack.m_positions[i];
        }
      }
    }

    void AnimatedTransformNode::read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      TransformNode::read(stream, eventManager, resourceHandles);

      unsigned int animationTracksSize;
      stream >> animationTracksSize;
      m_animationTracks.resize(animationTracksSize);

      for(unsigned int i = 0; i < m_animationTracks.size(); i++)
      {
        stream >> m_animationTracks[i];
      }

      stream >> m_animatedMesh.index;
      m_animatedMesh.nodeType = m_animatedMesh.index != ~0 ? ANIMATEDGEONODE : TREENODE;

      stream >> m_boneIndex;

      m_currentTrack = 0;
      m_currentAnimationTimeInSeconds = 0.0f;
      m_pauseAnimation = false;

      m_animatedTranslation = util::Vector<float, 3>::identity();
      m_animatedRotation = util::Quaternion<float>::identity();
      m_animatedScale = 1.0f;

      m_currentScaleKey = 0;
      m_currentPositionKey = 0;
      m_currentRotationKey = 0;
    }

    void AnimatedTransformNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
    {
      TransformNode::write(stream, resourceHandles);

      stream << m_animationTracks.size() << std::endl;
      for(unsigned int i = 0; i < m_animationTracks.size(); i++)
      {
        stream << m_animationTracks[i];
      }
      stream << m_animatedMesh.index << std::endl;
      stream << m_boneIndex << std::endl;
    }
  }
}