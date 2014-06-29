#ifndef NODEWRAPPERMAPPER_H_
#define NODEWRAPPERMAPPER_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>

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
      ~NodeWrapperMapper()
      {
        for(unsigned int i = 0; i < treeNodes.size(); i++)
        {
          delete treeNodes[i];
        }

        for(std::map<sg::TreeNode*, unsigned int>::iterator it = nodeMap.begin(); it != nodeMap.end(); it++)
        {
          delete it->first;
        }
      }

      class Less
      {
      public:
        inline bool operator()(const util::ResourceHandle& o1, const util::ResourceHandle& o2) const
        {
          return o1.getID() < o2.getID();
        }
      };

      std::map<sg::TreeNode*, unsigned int> nodeMap;//index into the treenodes vector, ptr as unique key
      std::vector<TreeNodeData*> treeNodes;//all treenodes

      std::map<std::string, std::map<util::ResourceHandle, std::string, Less>> resourceMap;//first string: resource kind e.g. mesh, billboard etc. | resource handle: for unique mapping | second string: filename
    };
  }
}

#endif