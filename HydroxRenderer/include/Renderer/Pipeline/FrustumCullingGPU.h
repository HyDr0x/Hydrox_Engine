#ifndef FRUSTUMCULLINGGPU_H_
#define FRUSTUMCULLINGGPU_H_

#include <vector>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Resources/ResourceManager.hpp"

namespace he
{
	namespace renderer
	{
    class RenderNode;

    class FrustumCullingGPU
    {
      public:

        FrustumCullingGPU();
        ~FrustumCullingGPU();

        void initialize(renderer::ComputeShaderManager *computeShaderManager, util::ResourceHandle frustumCullingShaderHandle, GLuint maxGeometry);

        void cullAABB(std::vector<unsigned int>& culledAABB, GLuint boundingBoxNumber);

      private:

        GPUBuffer m_culledAABBBuffer;

        util::ResourceHandle m_frustumCullingShaderHandle;
        renderer::ComputeShaderManager *m_computeShaderManager;
    };
	}
}

#endif
