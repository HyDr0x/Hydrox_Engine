#ifndef STATICGEOMETRYDECORATOR_H_
#define STATICGEOMETRYDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

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
      virtual bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual bool isEmpty();

      virtual void updateBuffer();

    protected:

      void resizeBuffer(unsigned int instanceNumber);
      void fillCaches(xBar::StaticGeometryContainer& geometryContainer, unsigned int instanceCounter);
      void fillBuffer(unsigned int instanceNumber);

      GPUBuffer m_matrixBuffer;
      std::vector<GLubyte> m_matrixCache;

      std::map<util::ResourceHandle, std::list<xBar::StaticGeometryContainer>, Less> m_meshHandles;
    };
	}
}

#endif
