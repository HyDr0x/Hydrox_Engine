#include "Renderer/Renderdata/SkinnedMatrixRenderData.h"

#include <vector>

#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
  namespace renderer
  {
    SkinnedMatrixRenderData::SkinnedMatrixRenderData(util::SharedPointer<RenderOptions> options) : IMatrixRenderData(options)
    {
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * m_options->maxBoneNumber * m_options->perInstanceBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_bboxMatrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * m_options->perInstanceBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    SkinnedMatrixRenderData::~SkinnedMatrixRenderData()
    {
    }

    void SkinnedMatrixRenderData::bindMatrixBuffer() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void SkinnedMatrixRenderData::unbindMatrixBuffer() const
    {
      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void SkinnedMatrixRenderData::updateBuffer(IInstanceRenderData& renderData)
    {
      unsigned int instanceNumber = renderData.getInstanceNumber();

      fillBuffer(renderData);

      GPUImmutableBuffer::syncWithWrittenData();
    }

    void SkinnedMatrixRenderData::fillBuffer(IInstanceRenderData& renderData)
    {
      unsigned int instanceIndex = 0;
      renderData.resetInstanceIterator();
      while(!renderData.isEndInstanceIterator())
      {
        const xBar::SkinnedGeometryContainer &skinnedGeometryContainer = dynamic_cast<const xBar::SkinnedGeometryContainer&>(renderData.incInstanceIterator());

        std::vector<util::Matrix<float, 4>>* boneTransformMatrices = skinnedGeometryContainer.getBoneTransformMatrices();
        std::vector<util::Matrix<float, 4>>* inverseBindPoseTransformMatrices = skinnedGeometryContainer.getInverseBindPoseTransformMatrices();

        std::vector<util::Matrix<float, 4>> skinningMatrices(boneTransformMatrices->size());

        for(unsigned int i = 0; i < skinningMatrices.size(); i++)
        {
          skinningMatrices[i] = (*boneTransformMatrices)[i] * (*inverseBindPoseTransformMatrices)[i];
        }

        unsigned int size = sizeof(util::Matrix<float, 4>) * skinningMatrices.size();
        unsigned int offset = sizeof(util::Matrix<float, 4>) * m_options->maxBoneNumber * instanceIndex;

        m_matrixBuffer.setData(offset, size, &(skinningMatrices[0][0][0]));
        m_bboxMatrixBuffer.setData(instanceIndex * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &skinnedGeometryContainer.getTransformationMatrix()[0][0]);

        instanceIndex++;
      }
    }
  }
}
