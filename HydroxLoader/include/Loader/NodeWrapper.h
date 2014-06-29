#ifndef LOADERNODEWRAPPER_H_
#define LOADERNODEWRAPPER_H_

#include <Utilities/Math/Math.hpp>

#include <SceneGraph/TreeNodes/TreeNode.h>
#include <SceneGraph/TreeNodes/GroupNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include <SceneGraph/TreeNodes/AnimationTrack.h>

#include <DataBase/Light.h>

namespace he
{
  namespace loader
  {
    //namespace sg
    //{
    //  class TransformNode;
    //  class AnimatedTransformNode;
    //  class GeoNode;
    //  class AnimatedGeoNode;
    //  class LODNode;
    //  class BillboardNode;
    //  class LightNode;
    //  class ShadowLightNode;
    //  class ParticleNode;
    //}

    struct TreeNodeData
    {
      TreeNodeData() : parentIndex(~0), firstChildIndex(~0), nextSiblingIndex(~0)
      {}

      sg::NodeType nodeType;
      std::string nodeName;

      sg::NodeType parentNodeType;
      unsigned int parentIndex;
      sg::GroupNode *parentNode;

      sg::NodeType firstChildNodeType;
      unsigned int firstChildIndex;
      sg::TreeNode *firstChildNode;

      sg::NodeType nextSiblingNodeType;
      unsigned int nextSiblingIndex;
      sg::TreeNode *nextSiblingNode;
    };

    struct GeoNodeData : public TreeNodeData
    {
      std::string meshFilename;
      std::string materialFilename;
    };

    struct AnimatedGeoNodeData : public GeoNodeData
    {
      std::vector<util::Matrix<float, 4>> inverseBindPoseMatrices;
    };

    struct TransformNodeData : public TreeNodeData
    {
      util::Quaternion<float> rotation;
      util::Vector<float, 3> translation;
      float scale;
    };

    struct AnimatedTransformNodeData : public TransformNodeData
    {
      std::vector<sg::AnimationTrack> animationTracks;
      unsigned int animatedMeshIndex;
      sg::AnimatedGeoNode* animatedGeoNode;

      unsigned int boneIndex;
    };

    struct LODNodeData : public TreeNodeData
    {
      util::Vector<float, 3> position;

      unsigned int lodLevel;
    };

    struct BillboardNodeData : public TreeNodeData
    {
      util::Vector<unsigned int, 2> animNumber;
      util::Vector<unsigned int, 2> animCount;
      util::Vector<float, 2> texStart;
      util::Vector<float, 2> texEnd;

      util::Vector<float, 2> scale;
      util::Vector<float, 3> translate;
      
      std::string textureFileName;
    };

    struct LightNodeData : public TreeNodeData
    {
      util::Vector<float, 4> color;

      float intensity;

      float spotLightExponent;
      float spotLightCutoff;

      float constAttenuation;
      float linearAttenuation;
      float quadricAttenuation;

      LightType lightType;
    };

    struct ShadowLightNodeData : public LightNodeData
    {
      util::Matrix<float, 4> projectionMatrix;
    };

    struct ParticleNodeData : public TreeNodeData
    {
      util::Matrix<float, 4> trfMatrix;
    };

    std::ifstream& operator >>(std::ifstream& stream, TreeNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, GeoNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, AnimatedGeoNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, TransformNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, AnimatedTransformNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, LODNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, BillboardNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, LightNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, ShadowLightNodeData& node);
    std::ifstream& operator >>(std::ifstream& stream, ParticleNodeData& node);
  }
}

#endif