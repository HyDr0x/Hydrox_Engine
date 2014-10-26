#ifndef IRENDERGROUP_H_
#define IRENDERGROUP_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <XBar/IGeometryContainer.h>

namespace he
{
  namespace renderer
  {
    class IRenderGroup
    {
    public:

      virtual ~IRenderGroup() = 0 {}

      virtual bool containsContainer(const xBar::IGeometryContainer& geometryContainer) = 0;
      virtual bool insertGeometry(const xBar::IGeometryContainer& geometryContainer) = 0;
      virtual bool removeGeometry(const xBar::IGeometryContainer& geometryContainer) = 0;

      virtual bool isEndInstanceIterator() const = 0;
      virtual void resetInstanceIterator() = 0;
      virtual const xBar::IGeometryContainer& incInstanceIterator() = 0;

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