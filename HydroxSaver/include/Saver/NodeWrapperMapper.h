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
      class Less
      {
      public:
        inline bool operator()(const util::ResourceHandle& o1, const util::ResourceHandle& o2) const
        {
          return o1.getID() < o2.getID();
        }
      };

      std::map<std::string, std::map<util::ResourceHandle, std::string, Less>> resourceMap;//first string: resource kind e.g. mesh, billboard etc. | resource handle: for unique mapping | second string: filename
    };
  }
}

#endif