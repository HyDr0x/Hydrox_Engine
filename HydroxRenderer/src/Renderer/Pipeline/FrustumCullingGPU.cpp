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

    void FrustumCullingGPU::initialize(renderer::ComputeShaderManager *computeShaderManager, util::ResourceHandle frustumCullingShaderHandle, GLuint maxGeometry)
    {
      m_frustumCullingShaderHandle = frustumCullingShaderHandle;

      m_computeShaderManager = computeShaderManager;

      m_culledAABBBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, maxGeometry * sizeof(unsigned int), 0, GL_STREAM_READ, nullptr);
    }

    void FrustumCullingGPU::cullAABB(std::vector<unsigned int>& culledAABB, GLuint boundingBoxNumber)
    {
      ComputeShader *frustumCullingShader = m_computeShaderManager->getObject(m_frustumCullingShaderHandle);

      m_culledAABBBuffer.bindBuffer(2);

      frustumCullingShader->setUniform(0, GL_UNSIGNED_INT, &boundingBoxNumber);

      frustumCullingShader->dispatchComputeShader(256, 1, 1);

      m_culledAABBBuffer.unbindBuffer(2);
        
      m_culledAABBBuffer.getData(0, boundingBoxNumber * sizeof(unsigned int), &culledAABB[0]);
    }
	}
}