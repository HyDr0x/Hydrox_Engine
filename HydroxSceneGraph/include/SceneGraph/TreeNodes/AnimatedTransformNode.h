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

      AnimatedTransformNode(){}
      AnimatedTransformNode(const std::vector<AnimationTrack>& animationTracks, const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0, NodeIndex firstChild = ~0);
      AnimatedTransformNode(const AnimatedTransformNode& sourceNode);
      AnimatedTransformNode(const TreeNode& sourceNode);
      virtual ~AnimatedTransformNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual GroupNode* clone() const;

      void initialize();

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      void setBoneIndex(unsigned int boneIndex);
      unsigned int getBoneIndex() const;

      void setSkinnedMesh(NodeIndex animatedMesh);
      NodeIndex getSkinnedMesh() const;

      void setCurrentAnimationTrack(unsigned int currentTrack);
      unsigned int getCurrentAnimationTrack() const;

      void setAnimationTracks(const std::vector<AnimationTrack>& animationTracks);
      const std::vector<AnimationTrack>& getAnimationTracks() const;

      void setCurrentAnimationTime(float time);
      void addCurrentAnimationTime(float time);

      void setPauseAnimation(bool pauseAnimation);
      bool getPauseAnimation() const;

      void stopAnimation();

      void calculateTransformation(util::vec3f& translation, float& scale, util::Quaternion<float>& rotation);

      //util::vec3f getGlobalPosition();
      //util::Quaternion<float> getGlobalRotation();
      //float getGlobalScale();

      util::vec3f getLocalPosition() const;
      util::Quaternion<float> getLocalRotation() const;
      float getLocalScale() const;

      virtual void read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::ResourceHandle::Less>>& resourceHandles) const;

    private:

      void interpolateKeyFrames(AnimationTrack& currentTrack);

      util::vec3f m_animatedTranslation;
      util::Quaternion<float> m_animatedRotation;
      float m_animatedScale;

      std::vector<AnimationTrack> m_animationTracks;//tha animation tracks for skeletal and skinned animation

      NodeIndex m_animatedMesh;//the corresponding animatedGeoNode if it is skinned

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
