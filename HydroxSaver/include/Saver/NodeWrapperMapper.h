#ifndef NODEWRAPPERMAPPER_H_
#define NODEWRAPPERMAPPER_H_

#include <DataBase/Mesh.h>
#include <DataBase/Material.h>
#include <DataBase/Texture2D.h>
#include <DataBase/RenderShader.h>

#include "Saver/NodeWrapper.h"

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
      std::map<sg::ShadowLightNode*, unsigned int> shadowLightNodeMap;
      std::map<sg::ParticleNode*, unsigned int> particleNodeMap;

      std::vector<GeoNodeData> geoNodes;
      std::vector<AnimatedGeoNodeData> animatedGeoNodes;
      std::vector<TransformNodeData> transformNodes;
      std::vector<AnimatedTransformNodeData> animatedTransformNodes;
      std::vector<LODNodeData> lodNodes;
      std::vector<BillboardNodeData> billboardNodes;
      std::vector<LightNodeData> lightNodes;
      std::vector<ShadowLightNodeData> shadowLightNodes;
      std::vector<ParticleNodeData> particleNodes;

      std::map<util::ResourceHandle, unsigned int, Less> meshMap;
      std::map<util::ResourceHandle, unsigned int, Less> materialMap;
      std::map<util::ResourceHandle, unsigned int, Less> billboardTextureMap;

      std::vector<db::Mesh> meshes;
      std::vector<std::string> materialFileNames;
      std::vector<std::string> billboardTextureFileNames;
    };
  }
}

#endif