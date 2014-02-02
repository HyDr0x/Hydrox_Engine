#ifndef FRUSTUMCULLINGGPU_H_
#define FRUSTUMCULLINGGPU_H_

#include <vector>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>

#include "Renderer/Buffer/SSBO.h"
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

        SSBO m_transformBuffer;
        SSBO m_bbMinBuffer;
        SSBO m_bbMaxBuffer;
        SSBO m_culledAABBBuffer;

        util::ResourceHandle m_frustumCullingShaderHandle;
        renderer::ComputeShaderManager *m_computeShaderManager;
    };
	}
}

#endif
