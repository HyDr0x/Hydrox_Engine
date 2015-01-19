#ifndef SKINNEDGEOMETRYDECORATOR_H_
#define SKINNEDGEOMETRYDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"
#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    class SkinnedGeometryDecorator : public ARenderNodeDecorator
    {
    public:

      SkinnedGeometryDecorator(IRenderGroup *renderNode, util::SharedPointer<RenderOptions> options);
      virtual ~SkinnedGeometryDecorator();

      virtual void frustumCulling() const;

      virtual void rasterizeShadowGeometry() const;

      virtual void rasterizeIndexGeometry() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

    protected:

      void fillBuffer();

      util::SharedPointer<RenderOptions> m_options;

      GPUImmutableBuffer m_matrixBuffer;
      GPUImmutableBuffer m_bboxMatrixBuffer;
    };
  }
}

#endif
