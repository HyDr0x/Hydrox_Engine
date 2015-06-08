#ifndef IMATRIXRENDERDATA_H_
#define IMATRIXRENDERDATA_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <XBar/IGeometryContainer.h>

#include "Renderer/Buffer/GPUImmutableBuffer.h"

#include "Renderer/Pipeline/RenderOptions.h"
#include "Renderer/Renderdata/IInstanceRenderData.h"

namespace he
{
  namespace renderer
  {
    class IMatrixRenderData
    {
    public:

      IMatrixRenderData(util::SharedPointer<RenderOptions> options) : m_options(options)
      {}

      virtual ~IMatrixRenderData() = 0 {}

      virtual void bindMatrixBuffer() const = 0;
      virtual void unbindMatrixBuffer() const = 0;

      virtual void updateBuffer(IInstanceRenderData& renderData) = 0;

    protected:

      util::SharedPointer<RenderOptions> m_options;

      GPUImmutableBuffer m_matrixBuffer;
    };
  }
}

#endif
