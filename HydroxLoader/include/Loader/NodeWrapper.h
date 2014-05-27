#ifndef LOADERNODEWRAPPER_H_
#define LOADERNODEWRAPPER_H_


#include <Utilities/Math/Math.hpp>

#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>
#include <SceneGraph/TreeNodes/AnimationTrack.h>

namespace he
{
  namespace loader
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
      util::Matrix<float, 4> trfMatrix;
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
    std::ifstream& operator >>(std::ifstream& stream, ParticleNodeData& node);
  }
}

#endif