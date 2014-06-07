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

      MaterialDecorator(IRenderNode *renderNode, util::SingletonManager *singletonManager);
      ~MaterialDecorator();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool insertGeometry(const xBar::IGeometryContainer& geometryContainer);
      virtual bool removeGeometry(const xBar::IGeometryContainer& geometryContainer);

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

    protected:

      void resizeMaterialBuffer();
      void resizeMaterialIndexBuffer();

      MaterialManager *m_materialManager;

      UBO m_materialBuffer;
      GPUImmutableBuffer m_materialIndexBuffer;

      bool m_materialNumberChanged;

      unsigned int m_materialCount;

      std::map<util::ResourceHandle, MaterialMemoryManager, Less> m_materialHandles;
    };
  }
}

#endif