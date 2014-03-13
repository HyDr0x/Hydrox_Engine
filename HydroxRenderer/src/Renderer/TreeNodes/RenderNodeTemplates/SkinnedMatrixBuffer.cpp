#include "Renderer/TreeNodes/RenderNodeTemplates/SkinnedMatrixBuffer.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
	namespace renderer
	{
    SkinnedMatrixBuffer::SkinnedMatrixBuffer(unsigned int maxBones) : m_maxBones(maxBones)
    {
    }

    SkinnedMatrixBuffer::~SkinnedMatrixBuffer()
    {
    }

    void SkinnedMatrixBuffer::bindTransformationMatrixBuffer()
    {
       m_matrixBuffer.bindBuffer(0);
    }

    void SkinnedMatrixBuffer::bindBBoxMatrixBuffer()
    {
       m_bboxMatrixBuffer.bindBuffer(0);
    }

    void SkinnedMatrixBuffer::unbindTransformationMatrixBuffer()
    {
       m_matrixBuffer.unbindBuffer(0);
    }

    void SkinnedMatrixBuffer::unbindBBoxMatrixBuffer()
    {
       m_bboxMatrixBuffer.unbindBuffer(0);
    }

    void SkinnedMatrixBuffer::resizeMatrixBuffer(unsigned int numInstances)
    {
      m_matrixCache.resize(sizeof(util::Matrix<float, 4>) * numInstances * m_maxBones);
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_matrixCache.size(), 0, GL_DYNAMIC_DRAW, nullptr);

      m_bboxMatrixCache.resize(sizeof(util::Matrix<float, 4>) * numInstances);
      m_bboxMatrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_bboxMatrixCache.size(), 0, GL_DYNAMIC_DRAW, nullptr);
    }

    void SkinnedMatrixBuffer::fillCaches(xBar::StaticGeometryContainer *geometryContainer, unsigned int instanceCounter)
    {
      xBar::SkinnedGeometryContainer *skinnedGeometryContainer = static_cast<xBar::SkinnedGeometryContainer*>(geometryContainer);

      std::vector<util::Matrix<float, 4>>& skinningMatrices = skinnedGeometryContainer->getSkinningMatrices();

      unsigned int size = sizeof(util::Matrix<float, 4>) * skinningMatrices.size();
      unsigned int offset = sizeof(util::Matrix<float, 4>) * m_maxBones * instanceCounter;

      memcpy(&m_matrixCache[offset], &(skinningMatrices[0][0][0]), size);

      memcpy(&m_bboxMatrixCache[instanceCounter * sizeof(util::Matrix<float, 4>)], &skinnedGeometryContainer->getTransformationMatrix()[0], sizeof(util::Matrix<float, 4>));
    }

    void SkinnedMatrixBuffer::fillBuffer(unsigned int numInstances)
    {
      m_matrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * m_maxBones * numInstances, &m_matrixCache[0]);
      m_bboxMatrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * numInstances, &m_bboxMatrixCache[0]);
    }
	}
}
