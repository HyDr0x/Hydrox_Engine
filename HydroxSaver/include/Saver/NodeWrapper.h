#ifndef NODEWRAPPER_H_
#define NODEWRAPPER_H_

#include <map>
#include <string>

#include <Utilities/Math/Math.hpp>

#include <SceneGraph/TreeNodes/TreeNode.h>
#include <SceneGraph/TreeNodes/GroupNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include <SceneGraph/TreeNodes/AnimationTrack.h>

#include <DataBase/Light.h>

namespace he
{
  namespace sg
  {
    class TransformNode;
    class AnimatedTransformNode;
    class GeoNode;
    class AnimatedGeoNode;
    class LODNode;
    class BillboardNode;
    class LightNode;
    class ShadowLightNode;
    class ParticleNode;
  }

  namespace saver
  {
    struct TreeNodeData
    {
      TreeNodeData() : parentIndex(~0), firstChildIndex(~0), nextSiblingIndex(~0)
      {}

      virtual ~TreeNodeData(){}

      virtual void convertPointerToIndices(std::map<sg::TreeNode*, unsigned int> nodeMap)
      {
        parentIndex = nodeMap[parentNode];
        firstChildIndex = nodeMap[firstChildNode];
        nextSiblingIndex = nodeMap[nextSiblingNode];
      }

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
      virtual void convertPointerToIndices(std::map<sg::TreeNode*, unsigned int> nodeMap)
      {
        parentIndex = nodeMap[parentNode];
        firstChildIndex = nodeMap[firstChildNode];
        nextSiblingIndex = nodeMap[nextSiblingNode];

        animatedMeshIndex = nodeMap[animatedGeoNode];
      }

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
      
      std::string textureFilename;
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

    std::ofstream& operator <<(std::ofstream& stream, const TreeNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const GeoNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const AnimatedGeoNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const TransformNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const AnimatedTransformNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const LODNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const BillboardNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const LightNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const ShadowLightNodeData& node);
    std::ofstream& operator <<(std::ofstream& stream, const ParticleNodeData& node);
  }
}

#endif