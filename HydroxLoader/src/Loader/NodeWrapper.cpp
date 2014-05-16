#include "Loader/NodeWrapper.h"

#include <fstream>

namespace he
{
  namespace loader
  {
    std::ifstream& operator >>(std::ifstream& stream, TreeNodeData& node)
    {
      unsigned int nodeTypes;

      stream >> node.nodeName;
      stream >> nodeTypes;
      node.parentNodeType = (NodeType)nodeTypes;
      stream >> node.parentIndex;
      stream >> nodeTypes;
      node.firstChildNodeType = (NodeType)nodeTypes;
      stream >> node.firstChildIndex;
      stream >> nodeTypes;
      node.nextSiblingNodeType = (NodeType)nodeTypes;
      stream >> node.nextSiblingIndex;

      return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, GeoNodeData& node)
    {
      stream >> (TreeNodeData&)node;
      stream >> node.meshIndex;
      stream >> node.materialIndex;

      return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, AnimatedGeoNodeData& node)
    {
      stream >> (GeoNodeData&)node;

      unsigned int inverseBindPoseMatricesSize;
      stream >> inverseBindPoseMatricesSize;
      node.inverseBindPoseMatrices.resize(inverseBindPoseMatricesSize);

      for(unsigned int i = 0; i < node.inverseBindPoseMatrices.size(); i++)
      {
        stream >> node.inverseBindPoseMatrices[i];
      }

      return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, TransformNodeData& node)
    {
      stream >> (TreeNodeData&)node;
      stream >> node.rotation;
      stream >> node.translation;
      stream >> node.scale;

      return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, AnimatedTransformNodeData& node)
    {
      stream >> (TransformNodeData&)node;

      unsigned int animationTracksSize;
      stream >> animationTracksSize;
      node.animationTracks.resize(animationTracksSize);

      for(unsigned int i = 0; i < node.animationTracks.size(); i++)
      {
        stream >> node.animationTracks[i];
      }
      stream >> node.animatedMeshIndex;
      stream >> node.boneIndex;

      return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, LODNodeData& node)
    {
      stream >> (TreeNodeData&)node;
      stream >> node.position;
      stream >> node.lodLevel;

      return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, BillboardNodeData& node)
    {
      stream >> (TreeNodeData&)node;
      stream >> node.animNumber;
      stream >> node.animCount;
      stream >> node.texStart;
      stream >> node.texEnd;
      stream >> node.scale;
      stream >> node.translate;
      stream >> node.textureIndex;

      return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, LightNodeData& node)
    {
      stream >> (TreeNodeData&)node;
      stream >> node.trfMatrix;

      return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, ParticleNodeData& node)
    {
      stream >> (TreeNodeData&)node;
      stream >> node.trfMatrix;

      return stream;
    }
  }
}