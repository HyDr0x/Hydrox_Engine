#include "Renderer/Pipeline/FrustumCullingGPU.h"

#include <Renderer/Resources/ResourceManager.hpp>

namespace he
{
	namespace renderer
	{
    FrustumCullingGPU::FrustumCullingGPU()
    {
    }

    FrustumCullingGPU::~FrustumCullingGPU()
    {
    }

    void FrustumCullingGPU::initialize(renderer::ComputeShaderManager *computeShaderManager, util::ResourceHandle frustumCullingShaderHandle)
    {
      m_frustumCullingShaderHandle = frustumCullingShaderHandle;

      m_computeShaderManager = computeShaderManager;
    }

    void FrustumCullingGPU::cullAABB(std::vector<util::Matrix<float, 4>>& transformMatrices, std::vector<util::Vector<float, 4>>& bbMin, std::vector<util::Vector<float, 4>>& bbMax, std::vector<unsigned int>& culledAABB)
    {
      GLuint boundingBoxNumber = transformMatrices.size();

      if(boundingBoxNumber > 0)
      {
        m_transformBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, boundingBoxNumber * sizeof(util::Matrix<float, 4>), 0, GL_STREAM_DRAW, &transformMatrices[0][0][0]);
        m_bbMinBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, bbMin.size() * sizeof(util::Vector<float, 4>), 0, GL_STREAM_DRAW, &bbMin[0][0]);
        m_bbMaxBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, bbMax.size() * sizeof(util::Vector<float, 4>), 0, GL_STREAM_DRAW, &bbMax[0][0]);
        m_culledAABBBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, boundingBoxNumber * sizeof(unsigned int), 0, GL_STREAM_READ, nullptr);

        m_transformBuffer.bindBuffer(0);
        m_bbMinBuffer.bindBuffer(1);
        m_bbMaxBuffer.bindBuffer(2);
        m_culledAABBBuffer.bindBuffer(3);

        ComputeShader *frustumCullingShader = m_computeShaderManager->getObject(m_frustumCullingShaderHandle);

        frustumCullingShader->useShader();
        frustumCullingShader->setUniform(1, GL_UNSIGNED_INT, &boundingBoxNumber);

        frustumCullingShader->dispatchComputeShader(256, 1, 1);

        m_transformBuffer.unbindBuffer(0);
        m_bbMinBuffer.unbindBuffer(1);
        m_bbMaxBuffer.unbindBuffer(2);
        m_culledAABBBuffer.unbindBuffer(3);

        m_culledAABBBuffer.getData(0, boundingBoxNumber * sizeof(unsigned int), &culledAABB[0]);
      }
    }
	}
}