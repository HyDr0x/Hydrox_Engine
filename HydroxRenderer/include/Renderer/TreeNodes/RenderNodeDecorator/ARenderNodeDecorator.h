#ifndef ARENDERNODEDECORATOR_H_
#define ARENDERNODEDECORATOR_H_

#include <list>
#include <map>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace xBar
  {
    class SkinnedGeometryContainer;
    class StaticGeometryContainer;
  }

  namespace renderer
  {
    class DrawCommandInterface;
    class MatrixBufferInterface;

    class ARenderNodeDecorator : public IRenderGroup
    {
    public:

      ARenderNodeDecorator(IRenderGroup *renderNode);
      virtual ~ARenderNodeDecorator() = 0;

      virtual bool containsContainer(const xBar::IGeometryContainer& geometryContainer);

      virtual bool insertGeometry(const xBar::IGeometryContainer& geometryContainer);
      virtual bool removeGeometry(const xBar::IGeometryContainer& geometryContainer);

      virtual bool isEndInstanceIterator() const;
      virtual void resetInstanceIterator();
      virtual const xBar::IGeometryContainer& incInstanceIterator();

      virtual unsigned int getInstanceNumber() const;

      virtual bool hasInstanceNumberChanged() const;

      virtual void frustumCulling() const;

      virtual void rasterizeShadowGeometry() const;

      virtual void rasterizeReflectiveShadowGeometry() const;

      virtual void rasterizeIndexGeometry() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

      virtual unsigned int getCacheNumber() const;

    protected:

      IRenderGroup *m_renderNode;
    };
  }
}

#endif
