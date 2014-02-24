#include "Renderer/RenderTree/StaticRenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/RenderTree/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    StaticRenderNode::StaticRenderNode(unsigned int maxMaterials, unsigned int maxGeometry) : RenderNode(maxMaterials, maxGeometry)
    {
    }

    StaticRenderNode::~StaticRenderNode()
    {
    }

    bool StaticRenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void StaticRenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    TreeNode* StaticRenderNode::createNewNode(InsertGeometryTraverser* traverser)
    {
      return traverser->createNewNode(this);
    }

    GPUBuffer& StaticRenderNode::getTransformationMatrixBuffer()
    {
      return m_matrixBuffer;
    }

    void StaticRenderNode::resizeMatrixBuffer()
    {
      m_matrixCache.resize(sizeof(util::Matrix<float, 4>) * m_instanceCounter);
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * m_instanceCounter, 0, GL_DYNAMIC_DRAW, nullptr);
    }

    void StaticRenderNode::fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex)
    {
      memcpy(&m_matrixCache[geometryIndex * sizeof(util::Matrix<float, 4>)], &node->getTransformationMatrix()[0], sizeof(util::Matrix<float, 4>));
    }

    void StaticRenderNode::fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex)
    {
    }

    void StaticRenderNode::uploadMatrices()
    {
      m_matrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * m_instanceCounter, &m_matrixCache[0]);
    }
	}
}
