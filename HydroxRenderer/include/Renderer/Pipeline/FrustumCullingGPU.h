#ifndef FRUSTUMCULLINGGPU_H_
#define FRUSTUMCULLINGGPU_H_

#include <vector>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>

#include "Renderer/Buffer/GPUBuffer.h"

namespace he
{
	namespace renderer
	{
    class FrustumCullingGPU
    {
      public:

        FrustumCullingGPU();
        ~FrustumCullingGPU();

        void initialize(GLuint maxGeometry);

        void cullAABB(std::vector<unsigned int>& culledAABB, GLuint boundingBoxNumber);

      private:

        GPUBuffer m_culledAABBBuffer;
    };
	}
}

#endif
