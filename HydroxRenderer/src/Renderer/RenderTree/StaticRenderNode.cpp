#include "Renderer/RenderTree/StaticRenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/RenderTree/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    StaticRenderNode::StaticRenderNode()
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

    void StaticRenderNode::resizeMatrixBuffer()
    {
      m_matrixBuffer.createBuffer(sizeof(util::Matrix<float, 4>) * m_geoNodes.size(), GL_STATIC_DRAW);
    }

    void StaticRenderNode::fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex)
    {
      m_matrixBuffer.setData(&node->getTransformationMatrix(), geometryIndex * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
    }

    void StaticRenderNode::fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex)
    {
    }

    void StaticRenderNode::uploadMatrixBuffer(sg::AnimatedGeoNode *node, Shader *renderShader)
    {
    }

    void StaticRenderNode::uploadMatrixBuffer(sg::GeoNode *node, Shader *renderShader)
    {
      util::Matrix<float, 4> worldMatrix = node->getTransformationMatrix();
      renderShader->setUniform(17, GL_FLOAT_MAT4, &(worldMatrix[0][0]));
    }
	}
}
