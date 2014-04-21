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

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual unsigned int removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual bool isEmpty();

      virtual void updateBuffer();

    protected:

      GPUImmutableBuffer m_matrixBuffer;

      std::list<xBar::StaticGeometryContainer> m_geometryContainer;
    };
	}
}

#endif
