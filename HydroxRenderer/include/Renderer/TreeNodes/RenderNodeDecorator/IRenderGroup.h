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

      virtual bool containsContainer(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual bool insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;
      virtual bool removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual bool isEndInstanceIterator() const = 0;
      virtual void resetInstanceIterator() = 0;
      virtual const xBar::IGeometryContainer& incInstanceIterator() = 0;

      virtual unsigned int getInstanceNumber() const = 0;

      virtual bool hasInstanceNumberChanged() const = 0;

      virtual void frustumCulling() const = 0;

      virtual void rasterizeShadowGeometry() const = 0;

      virtual void rasterizeReflectiveShadowGeometry() const = 0;

      virtual void rasterizeIndexGeometry() const = 0;

      virtual void rasterizeIndirectLightingGeometry() const = 0;

      virtual void rasterizeGeometry() const = 0;

      virtual void updateBuffer() = 0;

      virtual unsigned int getCacheNumber() const = 0;
    };
  }
}

#endif
