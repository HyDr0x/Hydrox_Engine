#ifndef LOADERNODEWRAPPERMAPPER_H_
#define LOADERNODEWRAPPERMAPPER_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Loader/NodeWrapper.h"

namespace he
{
  namespace sg
  {
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
      ~NodeWrapperMapper()
      {
        for(unsigned int i = 0; i < treeNodes.size(); i++)
        {
          delete treeNodes[i];
        }

        for(std::map<unsigned int, sg::TreeNode*>::iterator it = treeNodeMap.begin(); it != treeNodeMap.end(); it++)
        {
          delete it->second;
        }
      }

      std::map<unsigned int, sg::TreeNode*> treeNodeMap;

      std::vector<TreeNodeData*> treeNodes;

      std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceMap;
    };
  }
}

#endif