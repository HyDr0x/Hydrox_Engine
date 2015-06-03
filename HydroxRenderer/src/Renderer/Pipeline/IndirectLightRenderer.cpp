#include "Renderer/Pipeline/IndirectLightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/RandomSequenceGenerator.h>
#include <Utilities/Timer/Timer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/GPUCode.h"

namespace he
{
  namespace renderer
  {
    IndirectLightRenderer::IndirectLightRenderer() : m_cacheNumber(0), m_gBufferSync(0)
    {
    }

    IndirectLightRenderer::~IndirectLightRenderer()
    {
    }

    void IndirectLightRenderer::initialize(util::SingletonManager *singletonManager)
    {
      m_options = singletonManager->getService<RenderOptions>();
      m_singletonManager = singletonManager;

      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_proxyLightTextureResolution = 0;

      m_depthBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
      m_indirectLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 16));

      m_indirectLightRenderQuad.setRenderTargets(m_depthBuffer, 1, m_indirectLightMap);

      createSamplingPattern();
    }

    void IndirectLightRenderer::createSamplingPattern()
    {
      std::vector<util::vec4f> samplingPattern;
      std::vector<float> debugSamplingPattern;

      unsigned int imageSize = 513;

      //util::RandomSequenceGenerator::generateDiscSequence(m_options->giLightSampleNumber, 0.5f, 2.0f, imageSize, samplingPattern, debugSamplingPattern);
      util::RandomSequenceGenerator::generateHaltonSequence(m_options->giLightSampleNumber, util::vec2ui(2, 3), 0.5f, imageSize, samplingPattern, debugSamplingPattern);

      m_samplingDebugTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(imageSize, imageSize, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16, &debugSamplingPattern[0]));

      m_samplingPatternBuffer.createBuffer(sizeof(samplingPattern[0]) * samplingPattern.size(), GL_STATIC_DRAW);
      m_samplingPatternBuffer.setData(0, sizeof(samplingPattern[0]) * samplingPattern.size(), &samplingPattern[0]);
      m_samplingPatternBuffer.uploadData();
    }

    void IndirectLightRenderer::updateBuffer(unsigned int cacheNumber, unsigned int proxyLightTextureResolution)
    {
      if(cacheNumber != m_cacheNumber)
      {
        m_cacheNumber = cacheNumber;

        m_globalCachePositionBuffer.createBuffer(GL_TEXTURE_BUFFER, sizeof(util::vec4f) * m_options->globalCacheBlockSize, sizeof(util::vec4f) * m_cacheNumber, GL_STATIC_DRAW, GL_RGBA32F);
        m_globalCacheNormalBuffer.createBuffer(GL_TEXTURE_BUFFER, sizeof(util::vec4f) * m_options->globalCacheBlockSize, sizeof(util::vec4f) * m_cacheNumber, GL_STATIC_DRAW, GL_RGBA32F);
        m_globalCacheAreaBuffer.createBuffer(GL_TEXTURE_BUFFER, 2 * m_options->globalCacheBlockSize, 2 * m_cacheNumber, GL_STATIC_DRAW, GL_R16F);

        if(proxyLightTextureResolution != m_proxyLightTextureResolution)
        {
          m_proxyLightTextureResolution = proxyLightTextureResolution;

          m_indirectLightPositionBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_proxyLightTextureResolution, m_proxyLightTextureResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
          m_indirectLightLuminousFluxBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_proxyLightTextureResolution, m_proxyLightTextureResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));

          m_zBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_proxyLightTextureResolution, m_proxyLightTextureResolution, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_R8, GL_RED, 1, 8));
        }
      }
    }

    void IndirectLightRenderer::calculateIndirectLight(
      util::SharedPointer<db::Texture3D> reflectiveShadowPosMaps,
      util::SharedPointer<db::Texture3D> reflectiveShadowNormalMaps,
      util::SharedPointer<db::Texture3D> reflectiveShadowLuminousFluxMaps,
      const GPUImmutableBuffer& reflectiveShadowLightBuffer) const
    {
      //util::SharedPointer<db::Texture2D> visibilityMap;

      //calculateIndirectProxyLightsCPU(m_cacheNumber, 
      //  m_samplingPatternBuffer, 
      //  m_zBuffer, 
      //  reflectiveShadowLightBuffer, 
      //  m_globalCachePositionBuffer, 
      //  m_globalCacheNormalBuffer, 
      //  m_globalCacheAreaBuffer, 
      //  reflectiveShadowPosMaps,
      //  reflectiveShadowNormalMaps,
      //  reflectiveShadowLuminousFluxMaps,
      //  visibilityMap);

      {
        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//wait until the cache data is written into the global cache image buffers

        //CPUTIMER("cpuCompute", 0)
        //GPUTIMER("gpuCompute", 1)

        const sh::ComputeShader& computeShader = m_shaderContainer->getComputeShader(sh::ShaderContainer::INDIRECTLIGHTPROXYLIGHTCREATION);

        glViewport(0, 0, m_proxyLightTextureResolution, m_proxyLightTextureResolution);
        computeShader.useShader();

        m_samplingPatternBuffer.bindBuffer(1);

        reflectiveShadowPosMaps->setTexture(0, 0);
        reflectiveShadowNormalMaps->setTexture(1, 1);
        reflectiveShadowLuminousFluxMaps->setTexture(2, 2);

        m_globalCachePositionBuffer.bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCacheNormalBuffer.bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCacheAreaBuffer.bindImageTexture(2, 0, GL_READ_ONLY, GL_R16F);

        m_indirectLightPositionBuffer->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA32F);
        m_indirectLightLuminousFluxBuffer->bindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA32F);

        m_zBuffer->setTexture(6, 6);

        GLuint reflectiveShadowMapNumber = reflectiveShadowPosMaps->getResolution()[2];
        sh::RenderShader::setUniform(7, GL_UNSIGNED_INT, &reflectiveShadowMapNumber);

        sh::RenderShader::setUniform(8, GL_UNSIGNED_INT, &m_cacheNumber);

        sh::RenderShader::setUniform(9, GL_UNSIGNED_INT, &m_proxyLightTextureResolution);

        sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        m_zBuffer->unsetTexture(6);

        m_indirectLightLuminousFluxBuffer->unbindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA32F);
        m_indirectLightPositionBuffer->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA32F);

        m_globalCacheAreaBuffer.unbindImageTexture(2, 0, GL_READ_ONLY, GL_R16F);
        m_globalCacheNormalBuffer.unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCachePositionBuffer.unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);

        reflectiveShadowLuminousFluxMaps->unsetTexture(2);
        reflectiveShadowNormalMaps->unsetTexture(1);
        reflectiveShadowPosMaps->unsetTexture(0);

        m_samplingPatternBuffer.unBindBuffer(1);

        computeShader.useNoShader();

        glViewport(0, 0, m_options->width, m_options->height);
      }
    }

    void IndirectLightRenderer::setBuffer(util::SharedPointer<db::Texture2D> depthBuffer)
    {
      //m_gBufferSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

      //GLenum state;
      //do
      //{
      //  state = glClientWaitSync(m_gBufferSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
      //} while(state != GL_CONDITION_SATISFIED && state != GL_ALREADY_SIGNALED);

      util::Vector<GLubyte, 4> zeros(util::Vector<GLubyte, 4>::identity());
      m_zBuffer->clearTexture(&zeros);

      m_indirectLightIndicesRenderQuad.setRenderTargets(depthBuffer, 0);

      util::vec4ui negativeZeros(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX);
      m_indirectLightIndicesRenderQuad.clearTargets(1.0f, std::vector<util::vec4ui>(6, negativeZeros), false);

      m_indirectLightIndicesRenderQuad.setWriteFrameBuffer();

      m_globalCachePositionBuffer.bindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCacheNormalBuffer.bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCacheAreaBuffer.bindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16F);

      m_zBuffer->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_R8UI);

      //glDeleteSync(m_gBufferSync);
    }

    void IndirectLightRenderer::unsetBuffer() const
    {
      m_zBuffer->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_R8UI);

      m_globalCacheAreaBuffer.unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16F);
      m_globalCacheNormalBuffer.unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCachePositionBuffer.unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);

      m_indirectLightIndicesRenderQuad.unsetWriteFrameBuffer();
    }

    void IndirectLightRenderer::setCacheAndProxyLights() const
    {
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//wait until the proxy light data is written into the image buffer

      util::vec4f clearColor(0.0f, 0.0f, 0.0f, 0.0f);
      m_indirectLightRenderQuad.clearTargets(1.0f, clearColor);

      m_indirectLightRenderQuad.setWriteFrameBuffer();

      m_globalCachePositionBuffer.bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
      m_globalCacheNormalBuffer.bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);

      m_indirectLightPositionBuffer->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightLuminousFluxBuffer->bindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
    }

    void IndirectLightRenderer::unsetCacheAndProxyLights() const
    {
      m_indirectLightLuminousFluxBuffer->unbindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightPositionBuffer->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);

      m_globalCacheNormalBuffer.unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_globalCachePositionBuffer.unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);

      m_indirectLightRenderQuad.unsetWriteFrameBuffer();
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectLightMap() const
    {
      //std::vector<util::vec4ui> data0(m_options->width * m_options->height);
      //m_frameCacheIndexMap->convertToTexture2D(0)->getTextureData(&data0[0]);
      //util::vec4ui id000 = data0[0];
      //util::vec4ui id060 = data0[60];

      //std::vector<util::vec4ui> data1(m_options->width * m_options->height);
      //m_frameCacheIndexMap->convertToTexture2D(1)->getTextureData(&data1[0]);
      //util::vec4ui id100 = data1[0];
      //util::vec4ui id160 = data1[60];

      //std::vector<util::vec4ui> data2(m_options->width * m_options->height);
      //m_frameCacheIndexMap->convertToTexture2D(2)->getTextureData(&data2[0]);
      //util::vec4ui id200 = data2[0];
      //util::vec4ui id260 = data2[60];

      //std::vector<util::vec4ui> data3(m_options->width * m_options->height);
      //m_frameCacheIndexMap->convertToTexture2D(3)->getTextureData(&data3[0]);
      //util::vec4ui id300 = data3[0];
      //util::vec4ui id360 = data3[60];

      //std::vector<util::vec4ui> data4(m_options->width * m_options->height);
      //m_frameCacheIndexMap->convertToTexture2D(4)->getTextureData(&data4[0]);
      //util::vec4ui id400 = data4[0];
      //util::vec4ui id460 = data4[60];

      //std::vector<util::vec4ui> data5(m_options->width * m_options->height);
      //m_frameCacheIndexMap->convertToTexture2D(5)->getTextureData(&data5[0]);
      //util::vec4ui id500 = data5[0];
      //util::vec4ui id560 = data5[60];

      //std::vector<util::vec4f> cachePos(m_proxyLightTextureResolution * m_proxyLightTextureResolution);
      //m_globalCachePositionBuffer->getTextureData(&cachePos[0]);

      //std::vector<util::vec4f> cacheNormal(m_proxyLightTextureResolution * m_proxyLightTextureResolution);
      //m_globalCacheNormalBuffer->getTextureData(&cacheNormal[0]);

      return m_indirectLightMap;//m_indirectLightMap;
      //return m_TMPTEXTURE;
      //return m_indirectLightPositionDBuffer;
      //return m_globalCacheNormalBuffer;
      //return m_zBuffer;
      //return m_frameCacheIndexMap->convertToTexture2D(3);
      //return m_indirectLightLuminousFluxDBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectPositionMap()
    {
      return m_indirectLightPositionBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectLuminousFluxMap()
    {
      return m_indirectLightLuminousFluxBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getZBuffer()
    {
      return m_zBuffer;
    }

    const TBO& IndirectLightRenderer::getGlobalCachePositionMap()
    {
      return m_globalCachePositionBuffer;
    }

    const TBO& IndirectLightRenderer::getGlobalCacheNormalMap()
    {
      return m_globalCacheNormalBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getSamplingDebugMap()
    {
      return m_samplingDebugTexture;
    }
  }
}
