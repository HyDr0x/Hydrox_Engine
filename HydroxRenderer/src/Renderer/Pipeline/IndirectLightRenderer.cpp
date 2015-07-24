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
    struct DispatchIndirectCommand
    {
      unsigned int numGroupX;
      unsigned int numGroupY;
      unsigned int numGroupZ;
    };

    IndirectLightRenderer::IndirectLightRenderer() : m_cacheNumber(0), m_gBufferSync(0)
    {
      m_ping = 0;
      m_pong = 1 - m_ping;
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

      m_proxyWeightCreationShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::INDIRECTLIGHTPROXYLIGHTWEIGHTCREATIONPOST, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));

      m_depthBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
      m_indirectLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 16));

      m_indirectLightRenderQuad.setRenderTargets(m_depthBuffer, 1, m_indirectLightMap);

      DispatchIndirectCommand command;
      command.numGroupX = 128;
      command.numGroupY = 1;
      command.numGroupZ = 1;

      m_proxyLightCommandBuffer.createBuffer(GL_DISPATCH_INDIRECT_BUFFER, sizeof(DispatchIndirectCommand), sizeof(DispatchIndirectCommand), GL_STATIC_DRAW, &command);

      createSamplingPattern();
    }

    void IndirectLightRenderer::createSamplingPattern()
    {
      std::vector<util::vec4f> samplingPattern;
      std::vector<float> debugSamplingPattern;

      unsigned int imageSize = 513;

      //util::RandomSequenceGenerator::generateDiscSequence(m_options->giLightSampleNumber, 0.5f, 2.0f, imageSize, samplingPattern, debugSamplingPattern);
      util::RandomSequenceGenerator::generateHaltonSequence(m_options->giLightSampleNumber, util::vec2ui(7, 19), 0.5f, imageSize, samplingPattern, debugSamplingPattern);

      m_samplingDebugTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(imageSize, imageSize, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16, &debugSamplingPattern[0]));

      m_samplingPatternBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(samplingPattern[0]) * samplingPattern.size(), sizeof(samplingPattern[0]) * samplingPattern.size(), GL_STATIC_DRAW, &samplingPattern[0]);
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

          for(unsigned int i = 0; i < 4; i++)
          {
            m_indirectLightPositionBuffer[i] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_proxyLightTextureResolution, m_proxyLightTextureResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
            m_indirectLightLuminousFluxBuffer[i] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_proxyLightTextureResolution, m_proxyLightTextureResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
          }

          m_zBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_proxyLightTextureResolution, m_proxyLightTextureResolution, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_R8, GL_RED, 1, 8));
        }
      }
    }

    void IndirectLightRenderer::calculateIndirectLight(
      std::vector<util::SharedPointer<db::Texture2D>> reflectiveShadowPosLuminousFluxMaps,
      std::vector<util::SharedPointer<db::Texture2D>> reflectiveShadowNormalAreaMaps,
      const GPUImmutableBuffer& reflectiveShadowLightBuffer)
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

      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//wait until the cache data is written into the global cache image buffers

      //CPUTIMER("cpuCompute", 0)
      //GPUTIMER("gpuCompute", 1)

      unsigned int loopNumber = ceil(m_options->giLightSampleNumber / float(m_options->kCache));

      glViewport(0, 0, m_proxyLightTextureResolution, m_proxyLightTextureResolution);

      {
        const sh::RenderShader& computeShader = m_shaderContainer->getRenderShader(m_proxyWeightCreationShaderHandle);
        computeShader.useShader();

        util::vec4f clearData(0.0f);

        for(unsigned int i = 0; i < 4; i++)
        {
          m_indirectLightPositionBuffer[i]->clearTexture(&clearData[0]);
          m_indirectLightLuminousFluxBuffer[i]->clearTexture(&clearData[0]);
        }

        m_samplingPatternBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

        m_globalCachePositionBuffer.bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCacheNormalBuffer.bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);

        m_zBuffer->setTexture(2, 2);

        unsigned int reflectiveShadowLightNumber = reflectiveShadowPosLuminousFluxMaps[0]->getResolution()[2];
        sh::RenderShader::setUniform(3, GL_UNSIGNED_INT, &reflectiveShadowLightNumber);

        sh::RenderShader::setUniform(4, GL_UNSIGNED_INT, &m_cacheNumber);

        sh::RenderShader::setUniform(5, GL_UNSIGNED_INT, &m_proxyLightTextureResolution);

        for(unsigned int j = 0; j < reflectiveShadowPosLuminousFluxMaps.size(); j++)
        {
          reflectiveShadowPosLuminousFluxMaps[j]->setTexture(0, 0);
          reflectiveShadowNormalAreaMaps[j]->setTexture(1, 1);

          for(unsigned int i = 0; i < loopNumber; i++)
          {
            m_indirectProxyLightRenderQuad.setRenderTargets(4, m_indirectLightPositionBuffer[2 * m_pong], m_indirectLightPositionBuffer[2 * m_pong + 1], m_indirectLightLuminousFluxBuffer[2 * m_pong], m_indirectLightLuminousFluxBuffer[2 * m_pong + 1]);
            m_indirectProxyLightRenderQuad.setWriteFrameBuffer();

            m_indirectLightPositionBuffer[2 * m_ping]->setTexture(7, 7);
            m_indirectLightPositionBuffer[2 * m_ping + 1]->setTexture(8, 8);
            m_indirectLightLuminousFluxBuffer[2 * m_ping]->setTexture(9, 9);
            m_indirectLightLuminousFluxBuffer[2 * m_ping + 1]->setTexture(10, 10);

            unsigned int globalCacheOffset = i * m_options->kCache;
            sh::RenderShader::setUniform(6, GL_UNSIGNED_INT, &globalCacheOffset);

            m_indirectProxyLightRenderQuad.render();

            if(i + 1 < loopNumber || j + 1 < reflectiveShadowPosLuminousFluxMaps.size())
            {
              m_ping = m_pong;
              m_pong = 1 - m_ping;
            }

            m_indirectProxyLightRenderQuad.unsetWriteFrameBuffer();
            //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
          }

          reflectiveShadowNormalAreaMaps[j]->unsetTexture(1);
          reflectiveShadowPosLuminousFluxMaps[j]->unsetTexture(0);
        }

        m_zBuffer->unsetTexture(6);

        m_indirectLightPositionBuffer[2 * m_ping]->unsetTexture(7);
        m_indirectLightPositionBuffer[2 * m_ping + 1]->unsetTexture(8);
        m_indirectLightLuminousFluxBuffer[2 * m_ping]->unsetTexture(9);
        m_indirectLightLuminousFluxBuffer[2 * m_ping + 1]->unsetTexture(10);

        m_globalCacheNormalBuffer.unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCachePositionBuffer.unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);

        m_samplingPatternBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

        computeShader.useNoShader();
      }

      {
        const sh::ComputeShader& computeShader = m_shaderContainer->getComputeShader(sh::ShaderContainer::INDIRECTLIGHTPROXYLIGHTCREATION);
        computeShader.useShader();

        m_samplingPatternBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

        m_globalCachePositionBuffer.bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCacheNormalBuffer.bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);

        m_indirectLightPositionBuffer[2 * m_pong]->bindImageTexture(2, 0, GL_READ_WRITE, GL_RGBA32F);
        m_indirectLightPositionBuffer[2 * m_pong + 1]->bindImageTexture(3, 0, GL_READ_WRITE, GL_RGBA32F);
        m_indirectLightLuminousFluxBuffer[2 * m_pong]->bindImageTexture(4, 0, GL_READ_WRITE, GL_RGBA32F);
        m_indirectLightLuminousFluxBuffer[2 * m_pong + 1]->bindImageTexture(5, 0, GL_READ_WRITE, GL_RGBA32F);

        m_zBuffer->setTexture(0, 0);

        sh::RenderShader::setUniform(1, GL_UNSIGNED_INT, &m_cacheNumber);

        sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &m_proxyLightTextureResolution);

        sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        m_zBuffer->unsetTexture(0);

        m_indirectLightPositionBuffer[2 * m_pong]->unbindImageTexture(2, 0, GL_READ_WRITE, GL_RGBA32F);
        m_indirectLightPositionBuffer[2 * m_pong + 1]->unbindImageTexture(3, 0, GL_READ_WRITE, GL_RGBA32F);
        m_indirectLightLuminousFluxBuffer[2 * m_pong]->unbindImageTexture(4, 0, GL_READ_WRITE, GL_RGBA32F);
        m_indirectLightLuminousFluxBuffer[2 * m_pong + 1]->unbindImageTexture(5, 0, GL_READ_WRITE, GL_RGBA32F);

        m_globalCacheNormalBuffer.unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCachePositionBuffer.unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);

        m_samplingPatternBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

        computeShader.useNoShader();
      }

      glViewport(0, 0, m_options->width, m_options->height);
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

      m_indirectLightPositionBuffer[2 * m_pong]->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightPositionBuffer[2 * m_pong + 1]->bindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightLuminousFluxBuffer[2 * m_pong]->bindImageTexture(4, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightLuminousFluxBuffer[2 * m_pong + 1]->bindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);

      m_samplingPatternBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    }

    void IndirectLightRenderer::unsetCacheAndProxyLights() const
    {
      m_samplingPatternBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      m_indirectLightPositionBuffer[2 * m_pong]->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightPositionBuffer[2 * m_pong + 1]->unbindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightLuminousFluxBuffer[2 * m_pong]->unbindImageTexture(4, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightLuminousFluxBuffer[2 * m_pong + 1]->unbindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);

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

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectDiffusePositionMap()
    {
      return m_indirectLightPositionBuffer[2 * m_pong];
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectDiffuseLuminousFluxMap()
    {
      return m_indirectLightLuminousFluxBuffer[2 * m_pong];
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectSpecularPositionMap()
    {
      return m_indirectLightPositionBuffer[2 * m_pong + 1];
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectSpecularLuminousFluxMap()
    {
      return m_indirectLightLuminousFluxBuffer[2 * m_pong + 1];
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

    const TBO& IndirectLightRenderer::getGlobalCacheAreaMap()
    {
      return m_globalCacheAreaBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getSamplingDebugMap()
    {
      return m_samplingDebugTexture;
    }

    const GPUBuffer& IndirectLightRenderer::getSamplingBuffer()
    {
      return m_samplingPatternBuffer;
    }
  }
}
