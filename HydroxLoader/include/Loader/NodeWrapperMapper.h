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
      class Less
      {
      public:
        inline bool operator()(const util::ResourceHandle& o1, const util::ResourceHandle& o2) const
        {
          return o1.getID() < o2.getID();
        }
      };

      std::map<sg::GeoNode*, unsigned int> geoNodeMap;
      std::map<sg::AnimatedGeoNode*, unsigned int> animatedGeoNodeMap;
      std::map<sg::TransformNode*, unsigned int> transformNodeMap;
      std::map<sg::AnimatedTransformNode*, unsigned int> animatedTransformNodeMap;
      std::map<sg::LODNode*, unsigned int> lodNodeMap;
      std::map<sg::BillboardNode*, unsigned int> billboardNodeMap;
      std::map<sg::LightNode*, unsigned int> lightNodeMap;
      std::map<sg::ParticleNode*, unsigned int> particleNodeMap;

      std::vector<GeoNodeData> geoNodes;
      std::vector<AnimatedGeoNodeData> animatedGeoNodes;
      std::vector<TransformNodeData> transformNodes;
      std::vector<AnimatedTransformNodeData> animatedTransformNodes;
      std::vector<LODNodeData> lodNodes;
      std::vector<BillboardNodeData> billboardNodes;
      std::vector<LightNodeData> lightNodes;
      std::vector<ParticleNodeData> particleNodes;

      std::map<unsigned int, util::ResourceHandle, Less> meshMap;
      std::map<unsigned int, util::ResourceHandle, Less> materialMap;
      std::map<unsigned int, util::ResourceHandle, Less> billboardTextureMap;
    };
  }
}

#endif