#ifndef LOADERNODEWRAPPERMAPPER_H_
#define LOADERNODEWRAPPERMAPPER_H_

#include <DataBase/Mesh.h>
#include <DataBase/Material.h>
#include <DataBase/Texture2D.h>
#include <DataBase/RenderShader.h>

#include "Loader/NodeWrapper.h"

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
    class ParticleNode;
  }

  namespace loader
  {
    struct NodeWrapperMapper
    {
      std::map<unsigned int, sg::GeoNode*> geoNodeMap;
      std::map<unsigned int, sg::AnimatedGeoNode*> animatedGeoNodeMap;
      std::map<unsigned int, sg::TransformNode*> transformNodeMap;
      std::map<unsigned int, sg::AnimatedTransformNode*> animatedTransformNodeMap;
      std::map<unsigned int, sg::BillboardNode*> billboardNodeMap;
      std::map<unsigned int, sg::LODNode*> lodNodeMap;
      std::map<unsigned int, sg::LightNode*> lightNodeMap;
      std::map<unsigned int, sg::ParticleNode*> particleNodeMap;

      std::vector<GeoNodeData> geoNodes;
      std::vector<AnimatedGeoNodeData> animatedGeoNodes;
      std::vector<TransformNodeData> transformNodes;
      std::vector<AnimatedTransformNodeData> animatedTransformNodes;
      std::vector<LODNodeData> lodNodes;
      std::vector<BillboardNodeData> billboardNodes;
      std::vector<LightNodeData> lightNodes;
      std::vector<ParticleNodeData> particleNodes;

      std::map<unsigned int, util::ResourceHandle> meshMap;
      std::map<unsigned int, util::ResourceHandle> materialMap;
      std::map<unsigned int, util::ResourceHandle> billboardTextureMap;
    };
  }
}

#endif