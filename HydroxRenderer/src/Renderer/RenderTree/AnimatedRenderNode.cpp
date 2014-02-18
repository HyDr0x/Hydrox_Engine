#include "Renderer/RenderTree/AnimatedRenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/RenderTree/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    AnimatedRenderNode::AnimatedRenderNode(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones) : RenderNode(maxMaterials, maxGeometry), m_maxBones(maxBones)
    {
    }

    AnimatedRenderNode::~AnimatedRenderNode()
    {
    }

    bool AnimatedRenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void AnimatedRenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    TreeNode* AnimatedRenderNode::createNewNode(InsertGeometryTraverser* traverser)
    {
      return traverser->createNewNode(this);
    }

    void AnimatedRenderNode::resizeMatrixBuffer()
    {
      m_matrixCache.resize(sizeof(util::Matrix<float, 4>) * m_instanceCount * m_maxBones);
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_matrixCache.size(), 0, GL_DYNAMIC_DRAW, nullptr);
    }

    void AnimatedRenderNode::fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex)
    {
    }

    void AnimatedRenderNode::fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex)
    {
      std::vector<util::Matrix<float, 4>>& skinningMatrices = node->getSkinningMatrices();
      unsigned int size = sizeof(util::Matrix<float, 4>) * skinningMatrices.size();
      unsigned int offset = sizeof(util::Matrix<float, 4>) * m_maxBones * geometryIndex;
      memcpy(&m_matrixCache[offset], &(skinningMatrices[0][0][0]), size);
    }

    void AnimatedRenderNode::uploadMatrices()
    {
      m_matrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * m_maxBones * m_instanceCount, &m_matrixCache[0]);
    }
	}
}
