#ifndef LOADERNODEWRAPPERMAPPER_H_
#define LOADERNODEWRAPPERMAPPER_H_

#include <Renderer/Resources/Mesh.h>
#include <Renderer/Resources/Material.h>
#include <Renderer/Resources/Texture2D.h>
#include <Renderer/Resources/RenderShader.h>

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