#include "Renderer/TreeNodes/RenderNodeDecorator/SkinnedGeometryDecorator.h"

#include <vector>

#include "Renderer/Traverser/Traverser.h"

#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
  namespace renderer
  {
    SkinnedGeometryDecorator::SkinnedGeometryDecorator(IRenderGroup *renderNode) : ARenderNodeDecorator(renderNode)
    {
    }

    SkinnedGeometryDecorator::~SkinnedGeometryDecorator()
    {
    }

    void SkinnedGeometryDecorator::frustumCulling() const
    {
      m_bboxMatrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->frustumCulling();

      m_bboxMatrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void SkinnedGeometryDecorator::rasterizeShadowGeometry() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeShadowGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void SkinnedGeometryDecorator::rasterizeGeometry() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void SkinnedGeometryDecorator::updateBuffer()
    {
      unsigned int instanceNumber = getInstanceNumber();

      if(hasInstanceNumberChanged())
      {
        resizeBuffer(instanceNumber);
      }

      m_matrixBuffer.setMemoryFence();
      m_bboxMatrixBuffer.setMemoryFence();

      fillBuffer();

      m_renderNode->updateBuffer();

      m_bboxMatrixBuffer.syncWithFence();
      m_matrixBuffer.syncWithFence();
    }

    void SkinnedGeometryDecorator::resizeBuffer(unsigned int instanceNumber)
    {
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * getMaxBones() * instanceNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_bboxMatrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * instanceNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    void SkinnedGeometryDecorator::fillBuffer()
    {
      unsigned int instanceIndex = 0;
      resetInstanceIterator();
      while (!isEndInstanceIterator())
      {
        const xBar::SkinnedGeometryContainer &skinnedGeometryContainer = dynamic_cast<const xBar::SkinnedGeometryContainer&>(incInstanceIterator());

        std::vector<util::Matrix<float, 4>>* boneTransformMatrices = skinnedGeometryContainer.getBoneTransformMatrices();
        std::vector<util::Matrix<float, 4>>* inverseBindPoseTransformMatrices = skinnedGeometryContainer.getInverseBindPoseTransformMatrices();

        std::vector<util::Matrix<float, 4>> skinningMatrices(boneTransformMatrices->size());

        for(unsigned int i = 0; i < skinningMatrices.size(); i++)
        {
          skinningMatrices[i] = (*boneTransformMatrices)[i] * (*inverseBindPoseTransformMatrices)[i];
        }

        unsigned int size = sizeof(util::Matrix<float, 4>) * skinningMatrices.size();
        unsigned int offset = sizeof(util::Matrix<float, 4>) * getMaxBones() * instanceIndex;

        m_matrixBuffer.setData(offset, size, &(skinningMatrices[0][0][0]));
        m_bboxMatrixBuffer.setData(instanceIndex * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &skinnedGeometryContainer.getTransformationMatrix()[0][0]);

        instanceIndex++;
      }
    }
  }
}
