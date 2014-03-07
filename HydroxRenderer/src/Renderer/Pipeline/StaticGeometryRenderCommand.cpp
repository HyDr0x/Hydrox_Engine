#include "Renderer/Pipeline/StaticGeometryRenderCommand.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    StaticGeometryRenderCommand::StaticGeometryRenderCommand()
    {
    }

    StaticGeometryRenderCommand::~StaticGeometryRenderCommand()
    {
    }

    void StaticGeometryRenderCommand::bindTransformationMatrixBuffer()
    {
       m_matrixBuffer.bindBuffer(0);
    }

    void StaticGeometryRenderCommand::bindBBoxMatrixBuffer()
    {
       m_matrixBuffer.bindBuffer(0);
    }

    void StaticGeometryRenderCommand::unbindTransformationMatrixBuffer()
    {
       m_matrixBuffer.unbindBuffer(0);
    }

    void StaticGeometryRenderCommand::unbindBBoxMatrixBuffer()
    {
       m_matrixBuffer.unbindBuffer(0);
    }

    void StaticGeometryRenderCommand::resizeMatrixBuffer(unsigned int numInstances)
    {
      m_matrixCache.resize(sizeof(util::Matrix<float, 4>) * numInstances);
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * numInstances, 0, GL_DYNAMIC_DRAW, nullptr);
    }

    void StaticGeometryRenderCommand::fillCaches(sg::GeoNode *geoNode, unsigned int instanceCounter)
    {
      memcpy(&m_matrixCache[instanceCounter * sizeof(util::Matrix<float, 4>)], &geoNode->getTransformationMatrix()[0], sizeof(util::Matrix<float, 4>));
    }

    void StaticGeometryRenderCommand::fillBuffer(unsigned int numInstances)
    {
      m_matrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * numInstances, &m_matrixCache[0]);
    }
	}
}
