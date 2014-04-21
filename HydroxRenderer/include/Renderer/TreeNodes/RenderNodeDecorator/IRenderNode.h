#ifndef IRENDERNODE_H_
#define IRENDERNODE_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/TreeNodes/TreeNode.h"

namespace he
{
  namespace xBar
  {
    class SkinnedGeometryContainer;
    class StaticGeometryContainer;
  }

	namespace renderer
	{
    class IRenderNode : public TreeNode
    {
    public:

      virtual bool preTraverse(Traverser* traverser) = 0;
      virtual void postTraverse(Traverser* traverser) = 0;

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer) = 0;
      virtual unsigned int removeGeometry(xBar::StaticGeometryContainer& geometryContainer) = 0;

      virtual unsigned int getInstanceNumber() = 0;

      virtual bool hasInstanceNumberChanged() = 0;

      virtual void frustumCulling() = 0;

      virtual void rasterizeGeometry() = 0;

      virtual void updateBuffer() = 0;

    protected:

      class Less
      {
      public:
        inline bool operator()(const util::ResourceHandle& o1, const util::ResourceHandle& o2) const
        {
          return o1.getID() < o2.getID();
        }
      };
    };
	}
}

#endif
