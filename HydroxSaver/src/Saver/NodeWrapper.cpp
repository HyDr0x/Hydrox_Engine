#include "Saver/NodeWrapper.h"

#include <fstream>

namespace he
{
  namespace saver
  {
    std::ofstream& operator <<(std::ofstream& stream, const TreeNodeData& node)
    {
      stream << node.nodeType << std::endl;
      stream << node.nodeName << std::endl;
      stream << node.parentNodeType << std::endl;
      stream << node.parentIndex << std::endl;
      stream << node.firstChildNodeType << std::endl;
      stream << node.firstChildIndex << std::endl;
      stream << node.nextSiblingNodeType << std::endl;
      stream << node.nextSiblingIndex << std::endl;

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const GeoNodeData& node)
    {
      stream << (const TreeNodeData&)node;
      stream << node.meshFilename << std::endl;
      stream << node.materialFilename << std::endl;

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const AnimatedGeoNodeData& node)
    {
      stream << (const GeoNodeData&)node;

      stream << node.inverseBindPoseMatrices.size() << std::endl;
      if(node.inverseBindPoseMatrices.size()) 
      {
        stream.write((char*)&node.inverseBindPoseMatrices[0][0][0], sizeof(node.inverseBindPoseMatrices[0]) * node.inverseBindPoseMatrices.size());
        stream << std::endl;
      }

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const TransformNodeData& node)
    {
      stream << (const TreeNodeData&)node;
      stream << node.rotation << std::endl;
      stream << node.translation << std::endl;
      stream << node.scale << std::endl;

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const AnimatedTransformNodeData& node)
    {
      stream << (const TreeNodeData&)node;

      stream << node.animationTracks.size() << std::endl;
      for(unsigned int i = 0; i < node.animationTracks.size(); i++)
      {
        stream << node.animationTracks[i];
      }
      stream << node.animatedMeshIndex << std::endl;
      stream << node.boneIndex << std::endl;

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const LODNodeData& node)
    {
      stream << (const TreeNodeData&)node;
      stream << node.position << std::endl;
      stream << node.lodLevel << std::endl;

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const BillboardNodeData& node)
    {
      stream << (const TreeNodeData&)node;
      stream << node.animNumber << std::endl;
      stream << node.animCount << std::endl;
      stream << node.texStart << std::endl;
      stream << node.texEnd << std::endl;
      stream << node.scale << std::endl;
      stream << node.translate << std::endl;
      stream << node.textureFilename << std::endl;

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const LightNodeData& node)
    {
      stream << (const TreeNodeData&)node;
      stream << node.color << std::endl;

      stream << node.intensity << std::endl;

      stream << node.spotLightExponent << std::endl;
      stream << node.spotLightCutoff << std::endl;

      stream << node.constAttenuation << std::endl;
      stream << node.linearAttenuation << std::endl;
      stream << node.quadricAttenuation << std::endl;

      stream << node.lightType << std::endl;

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const ShadowLightNodeData& node)
    {
      stream << (const LightNodeData&)node;
      stream << node.projectionMatrix << std::endl;

      return stream;
    }

    std::ofstream& operator <<(std::ofstream& stream, const ParticleNodeData& node)
    {
      stream << (const TreeNodeData&)node;
      stream << node.trfMatrix << std::endl;

      return stream;
    }
  }
}