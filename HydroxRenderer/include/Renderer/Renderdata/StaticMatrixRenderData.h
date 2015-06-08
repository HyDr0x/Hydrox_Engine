#ifndef STATICMATRIXRENDERDATA_H_
#define STATICMATRIXRENDERDATA_H_

#include "Renderer/Renderdata/IMatrixRenderData.h"
#include "Renderer/Buffer/GPUImmutableBuffer.h"

namespace he
{
  namespace renderer
  {
    class StaticMatrixRenderData : public IMatrixRenderData
    {
    public:

      StaticMatrixRenderData(util::SharedPointer<RenderOptions> options);
      virtual ~StaticMatrixRenderData();

      virtual void bindMatrixBuffer() const override;
      virtual void unbindMatrixBuffer() const override;

      virtual void updateBuffer(IInstanceRenderData& renderData) override;

    private:

    };
  }
}

#endif
