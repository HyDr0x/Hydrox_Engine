#ifndef STATICGEOMETRYDECORATOR_H_
#define STATICGEOMETRYDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"

namespace he
{
  namespace renderer
  {
    class StaticGeometryDecorator : public ARenderNodeDecorator
    {
    public:

      StaticGeometryDecorator(IRenderNode *renderNode);
      virtual ~StaticGeometryDecorator();

      virtual void frustumCulling() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

    protected:

      GPUImmutableBuffer m_matrixBuffer;
    };
  }
}

#endif
