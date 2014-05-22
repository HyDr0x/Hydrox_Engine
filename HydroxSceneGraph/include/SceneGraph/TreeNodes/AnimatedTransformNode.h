#ifndef ANIMATEDTRANSFORMNODE_H_
#define ANIMATEDTRANSFORMNODE_H_

#include <vector>

#include <Utilities/Math/Math.hpp>

#include "SceneGraph/DLLExport.h"
#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/AnimationTrack.h"

namespace he
{
	namespace sg
	{
    class Traverser;
    class AnimatedGeoNode;

    class GRAPHICAPI AnimatedTransformNode : public TransformNode
    {
    public:

      AnimatedTransformNode(const std::vector<AnimationTrack>& animationTracks, const std::string& nodeName, 
        GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);

      virtual ~AnimatedTransformNode();

      AnimatedTransformNode& operator=(const AnimatedTransformNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual GroupNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      virtual bool isAnimatedTransformNode() const;

      void setBoneIndex(unsigned int boneIndex);
      unsigned int getBoneIndex() const;

      void setSkinnedMesh(AnimatedGeoNode* animatedMesh);
      AnimatedGeoNode* getSkinnedMesh() const;

      void setCurrentAnimationTrack(unsigned int currentTrack);
      unsigned int getCurrentAnimationTrack() const;

      const std::vector<AnimationTrack>& getAnimationTracks() const;

      void setCurrentAnimationTime(float time);
      void addCurrentAnimationTime(float time);

      void setPauseAnimation(bool pauseAnimation);
      bool getPauseAnimation() const;

      void stopAnimation();

      void calculateTransformation(util::Vector<float, 3>& translation, float& scale, util::Quaternion<float>& rotation);

      util::Vector<float, 3> getGlobalPosition();
      util::Quaternion<float> getGlobalRotation();
	    float getGlobalScale();

      util::Vector<float, 3> getLocalPosition() const;
	    util::Quaternion<float> getLocalRotation() const;
	    float getLocalScale() const;

    private:

      void interpolateKeyFrames(AnimationTrack& currentTrack);

      util::Vector<float, 3> m_animatedTranslation;
      util::Quaternion<float> m_animatedRotation;
      float m_animatedScale;

      std::vector<AnimationTrack> m_animationTracks;//tha animation tracks for skeletal and skinned animation

      AnimatedGeoNode* m_animatedMesh;//the corresponding animatedGeoNode if it is skinned

      unsigned int m_currentScaleKey;
      unsigned int m_currentRotationKey;
      unsigned int m_currentPositionKey;

      float m_currentAnimationTimeInSeconds;
      unsigned int m_currentTrack;
      unsigned int m_boneIndex;//index into the bone vector of the animatedGeoNode if it is skinned
      bool m_pauseAnimation;
    };
	}
}

#endif
