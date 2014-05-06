#ifndef MATERIALDECORATOR_H_
#define MATERIALDECORATOR_H_

#include <utility>

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Buffer/UBO.h"

namespace he
{
	namespace renderer
	{
    struct MaterialMemoryManager
    {
      unsigned int bufferIndex;
      unsigned int instanceNumber;
    };

    class MaterialDecorator : public ARenderNodeDecorator
    {
    public:

      MaterialDecorator(IRenderNode *renderNode, unsigned int maxMaterials, util::SingletonManager *singletonManager);
      ~MaterialDecorator();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual unsigned int removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual void rasterizeGeometry();

      virtual void updateBuffer();

    protected:

      void resizeMaterialBuffer();
      void resizeMaterialIndexBuffer();

      MaterialManager *m_materialManager;

      UBO m_materialBuffer;
      GPUImmutableBuffer m_materialIndexBuffer;

      bool m_materialNumberChanged;

      unsigned int m_maxMaterials;
      unsigned int m_materialCount;

      std::map<util::ResourceHandle, MaterialMemoryManager, Less> m_materialHandles;
      std::list<util::ResourceHandle> m_materialHandlesPerInstance;
    };
  }
}

#endif