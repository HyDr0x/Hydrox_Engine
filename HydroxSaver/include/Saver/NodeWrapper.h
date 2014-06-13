#ifndef NODEWRAPPER_H_
#define NODEWRAPPER_H_

#include <Utilities/Math/Math.hpp>

#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>
#include <SceneGraph/TreeNodes/AnimationTrack.h>
#include <SceneGraph/TreeNodes/LightNode.h>

namespace he
{
  namespace saver
  {
    enum NodeType
    {
      GEONODE,
      ANIMATEDGEONODE,
      TRANSFORMNODE,
      ANIMATEDTRANSFORMNODE,
      BILLBOARDNODE,
      LODNODE,
      LIGHTNODE,
      PARTICLENODE,
    };

    struct TreeNodeData
    {
      TreeNodeData() : parentIndex(~0), firstChildIndex(~0), nextSiblingIndex(~0)
      {}

      std::string nodeName;

      NodeType parentNodeType;
      unsigned int parentIndex;

      NodeType firstChildNodeType;
      unsigned int firstChildIndex;

      NodeType nextSiblingNodeType;
      unsigned int nextSiblingIndex;
    };

    struct GeoNodeData : public TreeNodeData
    {
      unsigned int meshIndex;
      unsigned int materialIndex;
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
      
      unsigned textureIndex;
    };

    struct LightNodeData : public TreeNodeData
    {
      util::Vector<float, 3> position;
      util::Vector<float, 3> direction;

      float intensity;

      float spotLightExponent;
      float spotLightCutoff;

      float constAttenuation;
      float linearAttenuation;
      float quadricAttenuation;

      sg::LightType lightType;
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
    std::ofstream& operator <<(std::ofstream& stream, const ParticleNodeData& node);
  }
}

#endif