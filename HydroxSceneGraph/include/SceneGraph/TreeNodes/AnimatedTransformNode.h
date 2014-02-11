#ifndef ANIMATEDTRANSFORMNODE_H_
#define ANIMATEDTRANSFORMNODE_H_

#include <vector>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include "SceneGraph/TreeNodes/TransformNode.h"

namespace he
{
	namespace sg
	{
    class Traverser;
    class AnimatedGeoNode;

    struct AnimationTrack//an animation track, each track represents all the keys for one animation in one node
    {
      std::string m_animationName;
      float m_duration;
      float m_animationTicksPerSecond;

      std::vector<util::Vector<float, 3>> m_positions;
      std::vector<float> m_positionsTime;

      std::vector<util::Quaternion<float>> m_rotations;
      std::vector<float> m_rotationsTime;

      std::vector<float> m_scales;
      std::vector<float> m_scalesTime;
    };

    class GRAPHICAPI AnimatedTransformNode : public TransformNode
    {
    public:

      AnimatedTransformNode(const std::vector<AnimationTrack>& animationTracks, const std::string& nodeName, 
        GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);

      AnimatedTransformNode& operator=(const AnimatedTransformNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);
      virtual ~AnimatedTransformNode();

      virtual GroupNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool isAnimatedTransformNode();

      void setBoneIndex(unsigned int boneIndex);
      unsigned int getBoneIndex();

      void setSkinnedMesh(AnimatedGeoNode* animatedMesh);
      AnimatedGeoNode* getSkinnedMesh();

      void setCurrentAnimationTrack(unsigned int currentTrack);
      unsigned int getCurrentAnimationTrack();

      void setCurrentAnimationTime(float time);
      void addCurrentAnimationTime(float time);

      void setPauseAnimation(bool pauseAnimation);
      bool getPauseAnimation();

      void stopAnimation();

      void calculateTransformation(util::Vector<float, 3>& translation, float& scale, util::Quaternion<float>& rotation);

      util::Vector<float, 3> getPosition();
	    util::Quaternion<float> getRotation();
	    float getScale();

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

      float m_currentAnimationTime;
      unsigned int m_currentTrack;
      unsigned int m_boneIndex;//index into the bone vector of the animatedGeoNode if it is skinned
      bool m_pauseAnimation;
    };
	}
}

#endif
