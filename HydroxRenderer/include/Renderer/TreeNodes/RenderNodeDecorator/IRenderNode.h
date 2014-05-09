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

      virtual bool insertGeometry(const xBar::SkinnedGeometryContainer& geometryContainer) = 0;

      virtual bool insertGeometry(const xBar::StaticGeometryContainer& geometryContainer) = 0;
      virtual bool removeGeometry(const xBar::StaticGeometryContainer& geometryContainer) = 0;

      virtual const std::list<const xBar::StaticGeometryContainer*>& getInstances() const = 0;

      virtual unsigned int getInstanceNumber() const = 0;

      virtual bool hasInstanceNumberChanged() const = 0;

      virtual void frustumCulling() const = 0;

      virtual void rasterizeGeometry() const = 0;

      virtual void updateBuffer() = 0;

      virtual unsigned int getMaxGeometry() const = 0;
      virtual unsigned int getMaxMaterials() const = 0;
      virtual unsigned int getMaxBones() const = 0;

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
