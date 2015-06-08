#include "Renderer/Renderdata/StaticMatrixRenderData.h"

#include <XBar/StaticGeometryContainer.h>

#include "Renderer/Traverser/Traverser.h"

namespace he
{
  namespace renderer
  {
    StaticMatrixRenderData::StaticMatrixRenderData(util::SharedPointer<RenderOptions> options) : IMatrixRenderData(options)
    {
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * m_options->perInstanceBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    StaticMatrixRenderData::~StaticMatrixRenderData()
    {
    }

    void StaticMatrixRenderData::bindMatrixBuffer() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticMatrixRenderData::unbindMatrixBuffer() const
    {
      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticMatrixRenderData::updateBuffer(IInstanceRenderData& renderData)
    {
      unsigned int instanceNumber = renderData.getInstanceNumber();

      unsigned int instanceIndex = 0;
      renderData.resetInstanceIterator();
      while(!renderData.isEndInstanceIterator())
      {
        const xBar::StaticGeometryContainer &staticGeometryContainer = dynamic_cast<const xBar::StaticGeometryContainer&>(renderData.incInstanceIterator());
        m_matrixBuffer.setData(sizeof(util::Matrix<float, 4>) * instanceIndex, sizeof(util::Matrix<float, 4>), &staticGeometryContainer.getTransformationMatrix()[0][0]);

        instanceIndex++;
      }

      GPUImmutableBuffer::syncWithWrittenData();
    }
  }
}
