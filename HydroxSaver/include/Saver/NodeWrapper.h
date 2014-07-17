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
    //std::ofstream& operator <<(std::ofstream& stream, const TreeNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const GeoNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const AnimatedGeoNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const TransformNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const AnimatedTransformNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const LODNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const BillboardNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const LightNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const ShadowLightNodeData& node);
    //std::ofstream& operator <<(std::ofstream& stream, const ParticleNodeData& node);
  }
}

#endif