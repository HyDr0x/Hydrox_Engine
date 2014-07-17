#ifndef LOADERNODEWRAPPERMAPPER_H_
#define LOADERNODEWRAPPERMAPPER_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Loader/NodeWrapper.h"

namespace he
{
  namespace sg
  {
    struct NodeIndex;

    class TreeNode;
    class GroupNode;
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

  namespace loader
  {
    struct NodeWrapperMapper
    {
      std::map<sg::NodeIndex, sg::TreeNode*> treeNodeMap;

      std::vector<sg::TreeNode*> treeNodes;

      std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceMap;
    };
  }
}

#endif