#ifndef SKINNEDGEOMETRYDECORATOR_H_
#define SKINNEDGEOMETRYDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"

namespace he
{
  namespace renderer
  {
    class SkinnedGeometryDecorator : public ARenderNodeDecorator
    {
    public:

      SkinnedGeometryDecorator(IRenderGroup *renderNode);
      virtual ~SkinnedGeometryDecorator();

      virtual void frustumCulling() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

    protected:

      void resizeBuffer(unsigned int instanceNumber);
      void fillBuffer();

      GPUImmutableBuffer m_matrixBuffer;
      GPUImmutableBuffer m_bboxMatrixBuffer;
    };
  }
}

#endif
