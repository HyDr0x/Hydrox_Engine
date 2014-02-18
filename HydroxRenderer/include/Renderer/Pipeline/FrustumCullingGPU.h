#ifndef FRUSTUMCULLINGGPU_H_
#define FRUSTUMCULLINGGPU_H_

#include <vector>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Resources/ResourceManager.hpp"

#include <Utilities/Miscellaneous/ResourceHandle.h>

namespace he
{
	namespace renderer
	{
    class FrustumCullingGPU
    {
      public:

        FrustumCullingGPU();
        ~FrustumCullingGPU();

        void initialize(renderer::ComputeShaderManager *computeShaderManager, util::ResourceHandle frustumCullingShaderHandle);

        void cullAABB(std::vector<util::Matrix<float, 4>>& transformMatrices, std::vector<util::Vector<float, 4>>& bbMin, std::vector<util::Vector<float, 4>>& bbMax, std::vector<unsigned int>& culledAABB);

      private:

        GPUBuffer m_transformBuffer;
        GPUBuffer m_bbMinBuffer;
        GPUBuffer m_bbMaxBuffer;
        GPUBuffer m_culledAABBBuffer;

        util::ResourceHandle m_frustumCullingShaderHandle;
        renderer::ComputeShaderManager *m_computeShaderManager;
    };
	}
}

#endif
