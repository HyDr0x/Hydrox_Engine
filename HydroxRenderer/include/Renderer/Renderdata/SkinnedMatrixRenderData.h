#ifndef SKINNEDMATRIXRENDERDATA_H_
#define SKINNEDMATRIXRENDERDATA_H_

#include "Renderer/Renderdata/IMatrixRenderData.h"

namespace he
{
  namespace renderer
  {
    class SkinnedMatrixRenderData : public IMatrixRenderData
    {
    public:

      SkinnedMatrixRenderData(util::SharedPointer<RenderOptions> options);
      virtual ~SkinnedMatrixRenderData();

      virtual void bindMatrixBuffer() const override;
      virtual void unbindMatrixBuffer() const override;

      virtual void updateBuffer(IInstanceRenderData& renderData) override;

    private:

      void fillBuffer(IInstanceRenderData& renderData);

      GPUImmutableBuffer m_bboxMatrixBuffer;
    };
  }
}

#endif
