#ifndef IINSTANCERENDERDATA_H_
#define IINSTANCERENDERDATA_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Pointer/SharedPointer.h>

#include <XBar/IGeometryContainer.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    class RenderOptions;

    class IInstanceRenderData
    {
    public:

      IInstanceRenderData(util::SharedPointer<RenderOptions> options) : m_options(options)
      {}

      virtual ~IInstanceRenderData() = 0
      {}

      virtual bool containsContainer(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual bool canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual void insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;
      virtual void removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      inline virtual bool isEndInstanceIterator() const = 0;

      inline virtual void resetInstanceIterator() = 0;

      inline virtual const xBar::IGeometryContainer& incInstanceIterator() = 0;

      virtual unsigned int getInstanceNumber() const = 0;

      virtual bool hasInstanceNumberChanged() const = 0;

      virtual void frustumCulling() const = 0;

      virtual void updateBuffer() = 0;

    protected:

      bool m_instanceNumberChanged;

      util::SharedPointer<RenderOptions> m_options;
    };
  }
}

#endif
