#include "Renderer/RenderTree/AnimatedRenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/RenderTree/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    AnimatedRenderNode::AnimatedRenderNode() : m_maxBones(64)
    {
      glGenBuffers(1, &m_boneMatricesBuffer);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_boneMatricesBuffer);
      glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * m_maxBones, nullptr, GL_STATIC_DRAW);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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
      m_matrixBuffer.createBuffer(sizeof(util::Matrix<float, 4>) * m_geoNodes.size() * m_maxBones, GL_STATIC_DRAW);
    }

    void AnimatedRenderNode::fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex)
    {
    }

    void AnimatedRenderNode::fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex)
    {
      std::vector<util::Matrix<float, 4>> skinningMatrices = node->getSkinningMatrices();
      unsigned int size = sizeof(util::Matrix<float, 4>) * skinningMatrices.size();
      m_matrixBuffer.setData(&skinningMatrices, geometryIndex * size, size);
    }

    void AnimatedRenderNode::uploadMatrixBuffer(sg::GeoNode *node, Shader *renderShader)
    {
    }

    void AnimatedRenderNode::uploadMatrixBuffer(sg::AnimatedGeoNode *node, Shader *renderShader)
    {
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_boneMatricesBuffer);
      std::vector<util::Matrix<float, 4>> skinningMatrices = node->getSkinningMatrices();
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(util::Matrix<float, 4>) * skinningMatrices.size(), &(skinningMatrices[0][0][0]));
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_boneMatricesBuffer); 
    }
	}
}
