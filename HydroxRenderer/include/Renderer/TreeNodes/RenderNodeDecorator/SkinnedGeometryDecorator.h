#ifndef SKINNEDGEOMETRYDECORATOR_H_
#define SKINNEDGEOMETRYDECORATOR_H_

#include <vector>

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

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
      virtual bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual bool isEmpty();

      virtual void updateBuffer();

    protected:

      void resizeBuffer(unsigned int instanceNumber);
      void fillCaches(xBar::SkinnedGeometryContainer& geometryContainer, unsigned int instanceCounter);
      void fillBuffer(unsigned int instanceNumber);

      GPUBuffer m_matrixBuffer;
      GPUBuffer m_bboxMatrixBuffer;

      std::vector<GLubyte> m_matrixCache;
      std::vector<GLubyte> m_bboxMatrixCache;

      unsigned int m_maxBones;

      std::map<util::ResourceHandle, std::list<xBar::SkinnedGeometryContainer>, Less> m_meshHandles;
    };
	}
}

#endif
