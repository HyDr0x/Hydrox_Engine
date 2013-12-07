#ifndef ANIMATEDTRANSFORMNODE_H_
#define ANIMATEDTRANSFORMNODE_H_

#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"
#include "Hydrox/Utility/Tree/TransformNode.h"

namespace he
{
  class Traverser;
  class AnimatedGeoNode;

  struct AnimationTrack//an animation track, each track represents all the keys for one animation in one node
  {
    std::string m_animationName;
    float m_duration;
    float m_animationTicksPerSecond;

    std::vector<Vector<float, 3>> m_positions;
    std::vector<float> m_positionsTime;

    std::vector<Quaternion<float>> m_rotations;
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

    void calculateTransformation(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation);

    Vector<float, 3> getPosition();
	  Quaternion<float> getRotation();
	  float getScale();

    static void setAnimatedSceneTime(AnimatedTransformNode *node, float animationTime);
    static void addAnimatedSceneTime(AnimatedTransformNode *node, float animationTime);
    static void pauseAnimatedScene(AnimatedTransformNode *node, bool pauseAnimation);
    static void stopAnimatedScene(AnimatedTransformNode *node);

  private:

    void interpolateKeyFrames(AnimationTrack& currentTrack);

    Vector<float, 3> m_animatedTranslation;
    Quaternion<float> m_animatedRotation;
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

#endif
