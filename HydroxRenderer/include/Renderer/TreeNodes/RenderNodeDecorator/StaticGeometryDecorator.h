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
      ~StaticGeometryDecorator();

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual void updateBuffer();

    protected:

      GPUImmutableBuffer m_matrixBuffer;
    };
	}
}

#endif
