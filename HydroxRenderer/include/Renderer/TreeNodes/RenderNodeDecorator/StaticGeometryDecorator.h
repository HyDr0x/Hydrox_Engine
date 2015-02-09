#ifndef STATICGEOMETRYDECORATOR_H_
#define STATICGEOMETRYDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"
#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    class StaticGeometryDecorator : public ARenderNodeDecorator
    {
    public:

      StaticGeometryDecorator(IRenderGroup *renderNode, util::SharedPointer<RenderOptions> options);
      virtual ~StaticGeometryDecorator();

      virtual void frustumCulling() const;

      virtual void rasterizeShadowGeometry() const;

      virtual void rasterizeReflectiveShadowGeometry() const;

      virtual void rasterizeIndexGeometry() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

    protected:

      util::SharedPointer<RenderOptions> m_options;

      GPUImmutableBuffer m_matrixBuffer;
    };
  }
}

#endif
