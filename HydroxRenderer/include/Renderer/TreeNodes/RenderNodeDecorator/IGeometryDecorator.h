#ifndef IGEOMETRYDECORATOR_H_
#define IGEOMETRYDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"

namespace he
{
  namespace renderer
  {
    class IGeometryDecorator : public ARenderNodeDecorator
    {
    public:

      IGeometryDecorator(IRenderNode *renderNode);
      ~IGeometryDecorator();

      virtual void frustumCulling() const = 0;

      virtual void rasterizeGeometry() const = 0;

      virtual void updateBuffer() = 0;

    protected:

      GPUImmutableBuffer m_matrixBuffer;
    };
  }
}

#endif
