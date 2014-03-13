#include "Renderer/TreeNodes/RenderNodeTemplates/StaticMatrixBuffer.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include <XBar/StaticGeometryContainer.h>

namespace he
{
	namespace renderer
	{
    StaticMatrixBuffer::StaticMatrixBuffer()
    {
    }

    StaticMatrixBuffer::~StaticMatrixBuffer()
    {
    }

    void StaticMatrixBuffer::bindTransformationMatrixBuffer()
    {
       m_matrixBuffer.bindBuffer(0);
    }

    void StaticMatrixBuffer::bindBBoxMatrixBuffer()
    {
       m_matrixBuffer.bindBuffer(0);
    }

    void StaticMatrixBuffer::unbindTransformationMatrixBuffer()
    {
       m_matrixBuffer.unbindBuffer(0);
    }

    void StaticMatrixBuffer::unbindBBoxMatrixBuffer()
    {
       m_matrixBuffer.unbindBuffer(0);
    }

    void StaticMatrixBuffer::resizeMatrixBuffer(unsigned int numInstances)
    {
      m_matrixCache.resize(sizeof(util::Matrix<float, 4>) * numInstances);
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * numInstances, 0, GL_DYNAMIC_DRAW, nullptr);
    }

    void StaticMatrixBuffer::fillCaches(xBar::StaticGeometryContainer *geometryContainer, unsigned int instanceCounter)
    {
      memcpy(&m_matrixCache[instanceCounter * sizeof(util::Matrix<float, 4>)], &geometryContainer->getTransformationMatrix()[0], sizeof(util::Matrix<float, 4>));
    }

    void StaticMatrixBuffer::fillBuffer(unsigned int numInstances)
    {
      m_matrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * numInstances, &m_matrixCache[0]);
    }
	}
}
