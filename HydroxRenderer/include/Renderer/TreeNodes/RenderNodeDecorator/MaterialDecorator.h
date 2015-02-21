#ifndef MATERIALDECORATOR_H_
#define MATERIALDECORATOR_H_

#include <utility>

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    struct MaterialIndexData
    {
      unsigned int bufferIndex;
      unsigned int instanceNumber;
    };

    class MaterialDecorator : public ARenderNodeDecorator
    {
    public:

      MaterialDecorator(IRenderGroup *renderNode, util::SingletonManager *singletonManager);
      virtual ~MaterialDecorator();

      virtual bool insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer);
      virtual bool removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer);

      virtual void rasterizeReflectiveShadowGeometry() const;

      virtual void rasterizeIndexGeometry() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

    protected:

      void resizeMaterialBuffer();
      void resizeMaterialIndexBuffer();

      util::SharedPointer<db::MaterialManager>m_materialManager;
      util::SharedPointer<RenderOptions> m_options;

      UBO m_materialBuffer;
      GPUImmutableBuffer m_materialIndexBuffer;

      bool m_materialNumberChanged;

      unsigned int m_materialCount;

      std::map<util::ResourceHandle, MaterialIndexData, Less> m_materialHandles;
    };
  }
}

#endif