#include "Renderer/Pipeline/IndirectLightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Timer/Timer.h>

#include <DataBase/ComputeShader.h>
#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"
#include "Renderer/Pipeline/RenderShaderContainer.h"

namespace he
{
  namespace renderer
  {
    IndirectLightRenderer::IndirectLightRenderer() : 
      m_cacheNumber(0)
    {
    }

    IndirectLightRenderer::~IndirectLightRenderer()
    {
    }

    void IndirectLightRenderer::initialize(util::SingletonManager *singletonManager)
    {
      m_options = singletonManager->getService<RenderOptions>();
      m_singletonManager = singletonManager;

      m_indirectLightShaderHandle = singletonManager->getService<RenderShaderContainer>()->indirectLightShaderHandle;
      m_indirectLightInterpolationShaderHandle = singletonManager->getService<RenderShaderContainer>()->indirectLightInterpolationShaderHandle;

      m_globalCacheBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Cache) * m_options->globalCacheBlockSize, 1 * sizeof(util::Cache), GL_STATIC_DRAW, nullptr);
      m_zBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_options->globalCacheBlockSize, 1 * sizeof(util::Cache), GL_STATIC_DRAW, nullptr);
      m_indirectLightDataBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(IndirectLight) * m_options->globalCacheBlockSize * 2, 1 * sizeof(util::Cache), GL_STATIC_DRAW, nullptr);

      m_frameCacheIndexMap = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->width, m_options->height, 6, GL_TEXTURE_2D_ARRAY, GL_UNSIGNED_INT, GL_RGBA32UI, GL_RGBA_INTEGER, 4, 128));
      
      m_indirectLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64));

      m_indirectLightRenderQuad.setRenderTargets(1, m_indirectLightMap);
    }

    void IndirectLightRenderer::updateBuffer(unsigned int cacheNumber)
    {
      if(cacheNumber != m_cacheNumber)
      {
        m_cacheNumber = cacheNumber;
        m_globalCacheBuffer.resizeBuffer(m_cacheNumber * sizeof(util::Cache));
        m_zBuffer.resizeBuffer(m_cacheNumber * sizeof(GLuint));
        m_indirectLightDataBuffer.resizeBuffer(2 * m_cacheNumber * sizeof(IndirectLight));
      }
    }

    void IndirectLightRenderer::calculateIndirectLight(
      util::SharedPointer<db::Texture2D> depthMap,
      util::SharedPointer<db::Texture2D> normalMap,
      util::SharedPointer<db::Texture2D> materialMap,
      util::SharedPointer<db::Texture3D> reflectiveShadowPosMaps,
      util::SharedPointer<db::Texture3D> reflectiveShadowNormalMaps,
      util::SharedPointer<db::Texture3D> reflectiveShadowLuminousFluxMaps) const
    {
      {
        //CPUTIMER("cpuCompute", 0)
        GPUTIMER("gpuCompute", 1)

        db::ComputeShader *computeShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_indirectLightShaderHandle);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        computeShader->useShader();

        GLuint indirectLightNumberXY = m_options->shadowMapWidth;
        db::RenderShader::setUniform(3, GL_UNSIGNED_INT, &indirectLightNumberXY);

        GLuint indirectLightNumberZ = reflectiveShadowPosMaps->getResolution()[2];
        db::RenderShader::setUniform(4, GL_UNSIGNED_INT, &indirectLightNumberZ);

        db::RenderShader::setUniform(5, GL_UNSIGNED_INT, &m_cacheNumber);

        db::RenderShader::setUniform(6, GL_UNSIGNED_INT, &m_options->unusedLightIndirectNumber);

        reflectiveShadowPosMaps->setTexture(0, 0);
        reflectiveShadowNormalMaps->setTexture(1, 1);
        reflectiveShadowLuminousFluxMaps->setTexture(2, 2);

        m_globalCacheBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        m_zBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
        m_indirectLightDataBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

        db::ComputeShader::dispatchComputeShader(128, 1, 1);

        m_indirectLightDataBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
        m_zBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
        m_globalCacheBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        reflectiveShadowLuminousFluxMaps->unsetTexture();
        reflectiveShadowNormalMaps->unsetTexture();
        reflectiveShadowPosMaps->unsetTexture();

        computeShader->useNoShader();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      }
      {
        //CPUTIMER("cpuCompute", 0)
        //GPUTIMER("gpuCompute", 1)

        db::RenderShader *renderShader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_indirectLightInterpolationShaderHandle);

        renderShader->useShader();

        m_indirectLightRenderQuad.setWriteFrameBuffer();

        depthMap->setTexture(0, 0);
        normalMap->setTexture(1, 1);
        materialMap->setTexture(2, 2);

        m_frameCacheIndexMap->setTexture(3, 3);

        m_globalCacheBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        m_indirectLightDataBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

        m_indirectLightRenderQuad.render();

        m_indirectLightDataBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
        m_globalCacheBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_frameCacheIndexMap->unsetTexture();

        materialMap->unsetTexture();
        normalMap->unsetTexture();
        depthMap->unsetTexture();

        m_indirectLightRenderQuad.unsetWriteFrameBuffer();

        renderShader->useNoShader();
      }
    }

      void IndirectLightRenderer::setBuffer(util::SharedPointer<db::Texture2D> depthTexture)
    {
      m_indirectLightIndicesRenderQuad.setRenderTargets3D(depthTexture, 1, m_frameCacheIndexMap);

      GLuint zeros = 0;
      m_zBuffer.clearBuffer(GL_RED, GL_R32UI, GL_UNSIGNED_INT, &zeros);

      util::vec4ui negativeZeros(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX);
      m_indirectLightIndicesRenderQuad.clearTargets(1.0f, std::vector<util::vec4ui>(6, negativeZeros), false);

      m_indirectLightIndicesRenderQuad.setWriteFrameBuffer();

      m_globalCacheBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_zBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 9);
    }

    void IndirectLightRenderer::unsetBuffer() const
    {
      m_zBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 9);
      m_globalCacheBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

      m_indirectLightIndicesRenderQuad.unsetWriteFrameBuffer();
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectLightMap() const
    {
      return m_indirectLightMap;
      //return m_frameCacheIndexMap->convertToTexture2D(5);
    }

    GPUBuffer& IndirectLightRenderer::getIndirectLightsBuffer()
    {
      return m_indirectLightDataBuffer;
    }

    GPUBuffer& IndirectLightRenderer::getGlobalCacheBuffer()
    {
      return m_globalCacheBuffer;
    }
  }
}
