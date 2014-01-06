#ifndef FRUSTUMCULLINGGPU_H_
#define FRUSTUMCULLINGGPU_H_

#include <vector>

#include <GL/glew.h>

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Graphics/SSBO.h"

#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

namespace he
{
  class FrustumCullingGPU
  {
    public:

      FrustumCullingGPU();
      ~FrustumCullingGPU();

      void initialize();

      void cullAABB(std::vector<Matrix<float, 4>>& transformMatrices, std::vector<Vector<float, 4>>& bbMin, std::vector<Vector<float, 4>>& bbMax, std::vector<unsigned int>& culledAABB);

    private:

      SSBO m_transformBuffer;
      SSBO m_bbMinBuffer;
      SSBO m_bbMaxBuffer;
      SSBO m_culledAABBBuffer;

      ResourceHandle m_frustumCullingShaderHandle;
  };
}

#endif
