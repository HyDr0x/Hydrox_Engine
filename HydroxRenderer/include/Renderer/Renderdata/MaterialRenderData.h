#ifndef MATERIALRENDERDATA_H_
#define MATERIALRENDERDATA_H_

#include <utility>

#include "Renderer/Renderdata/IMaterialRenderData.h"

namespace he
{
  namespace renderer
  {
    class MaterialRenderData : public IMaterialRenderData
    {
    public:

      MaterialRenderData(util::SingletonManager *singletonManager, std::vector<std::vector<util::ResourceHandle>> textureHandles);
      virtual ~MaterialRenderData();

      virtual bool canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;

      virtual void insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;
      virtual void removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;

      virtual void bindMaterialBuffer() const override;
      virtual void unbindMaterialBuffer() const override;

      virtual const std::vector<std::vector<util::ResourceHandle>>& getTextureHandles() const override;

      virtual void updateBuffer(IInstanceRenderData& renderData) override;

    protected:

      void resizeMaterialBuffer(IInstanceRenderData& renderData);
      void resizeMaterialIndexBuffer(IInstanceRenderData& renderData);
    };
  }
}

#endif