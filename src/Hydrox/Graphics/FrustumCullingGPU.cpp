#include "Hydrox/Graphics/FrustumCullingGPU.h"

#include "Hydrox/Singletons/CacheManager.hpp"
#include "Hydrox/Loader/ComputeShaderLoader.h"

namespace he
{
  FrustumCullingGPU::FrustumCullingGPU()
  {
  }

  FrustumCullingGPU::~FrustumCullingGPU()
  {
  }

  void FrustumCullingGPU::initialize()
  {
    ComputeShaderLoader shaderLoader;

    m_frustumCullingShaderHandle = shaderLoader.loadShader(std::string("FrustumCullingShader"), std::string("frustumCulling.comp"));

    m_transformBuffer.setBindingPoint(0);
    m_bbMinBuffer.setBindingPoint(1);
    m_bbMaxBuffer.setBindingPoint(2);
    m_culledAABBBuffer.setBindingPoint(3);
  }

  void FrustumCullingGPU::cullAABB(std::vector<Matrix<float, 4>>& transformMatrices, std::vector<Vector<float, 4>>& bbMin, std::vector<Vector<float, 4>>& bbMax, std::vector<unsigned int>& culledAABB)
  {
    GLuint boundingBoxNumber = transformMatrices.size();

    m_transformBuffer.createBuffer(boundingBoxNumber * sizeof(Matrix<float, 4>), GL_STREAM_DRAW, &transformMatrices[0][0][0]);
    m_bbMinBuffer.createBuffer(boundingBoxNumber * sizeof(Vector<float, 4>), GL_STREAM_DRAW, &bbMin[0][0]);
    m_bbMaxBuffer.createBuffer(boundingBoxNumber * sizeof(Vector<float, 4>), GL_STREAM_DRAW, &bbMax[0][0]);
    m_culledAABBBuffer.createBuffer(boundingBoxNumber * sizeof(unsigned int), GL_STREAM_READ, nullptr);

    m_transformBuffer.bindBuffer();
    m_bbMinBuffer.bindBuffer();
    m_bbMaxBuffer.bindBuffer();
    m_culledAABBBuffer.bindBuffer();

    ComputeShaderManager *computeShaderManager = ComputeShaderManager::getInstance();

    ComputeShader *frustumCullingShader = computeShaderManager->getObject(m_frustumCullingShaderHandle);

    frustumCullingShader->useShader();
    frustumCullingShader->setUniform(1, GL_UNSIGNED_INT, &boundingBoxNumber);

    frustumCullingShader->dispatchComputeShader(256, 1, 1);

    m_transformBuffer.unBindBuffer();
    m_bbMinBuffer.unBindBuffer();
    m_bbMaxBuffer.unBindBuffer();
    m_culledAABBBuffer.unBindBuffer();

    m_culledAABBBuffer.getData(&culledAABB[0], 0, boundingBoxNumber * sizeof(unsigned int));
  }
}