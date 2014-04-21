#ifndef SKINNEDGEOMETRYDECORATOR_H_
#define SKINNEDGEOMETRYDECORATOR_H_

#include <vector>

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"

namespace he
{
	namespace renderer
	{
    class SkinnedGeometryDecorator : public ARenderNodeDecorator
    {
    public:

      SkinnedGeometryDecorator(IRenderNode *renderNode, unsigned int maxBones);
      ~SkinnedGeometryDecorator();

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual unsigned int removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual bool isEmpty();

      virtual void updateBuffer();

    protected:

      void resizeBuffer(unsigned int instanceNumber);
      void fillBuffer();

      GPUImmutableBuffer m_matrixBuffer;
      GPUImmutableBuffer m_bboxMatrixBuffer;

      unsigned int m_maxBones;

      std::list<xBar::SkinnedGeometryContainer> m_geometryContainer;
    };
	}
}

#endif
