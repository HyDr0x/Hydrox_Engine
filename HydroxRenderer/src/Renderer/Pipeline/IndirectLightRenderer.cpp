#include "Renderer/Pipeline/IndirectLightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Timer/Timer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/GPUCode.h"

namespace he
{
  namespace renderer
  {
    IndirectLightRenderer::IndirectLightRenderer() : m_cacheNumber(0)
    {
    }

    IndirectLightRenderer::~IndirectLightRenderer()
    {
    }

    void IndirectLightRenderer::initialize(util::SingletonManager *singletonManager)
    {
      m_options = singletonManager->getService<RenderOptions>();
      m_singletonManager = singletonManager;

      m_renderShaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_indirectLightShaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::INDIRECTLIGHTPROXYLIGHTCREATION, sh::ShaderSlotFlags(8192));

      m_bufferResolution = 0;

      m_depthBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
      m_indirectLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));

      m_indirectLightRenderQuad.setRenderTargets(m_depthBuffer, 1, m_indirectLightMap);

      createSamplingPattern();
    }

    void IndirectLightRenderer::createSamplingPattern()
    {
      const float rMax = 1.0f;

      std::vector<util::vec4f> samplingPattern(m_options->giLightSampleNumber);

      for(unsigned int i = 0; i < samplingPattern.size(); i++)
      {
        float s0 = rand() / float(RAND_MAX);
        float s1 = rand() / float(RAND_MAX);

        samplingPattern[i] = util::vec4f(rMax * s0 * sinf(2.0f * util::math::PI * s1), rMax * s0 * cosf(2.0f * util::math::PI * s1), (1.0f / float(m_options->giLightSampleNumber)), 0.0f);
        //samplingPattern[i] *= s0 * s0;
        //samplingPattern[i] = samplingPattern[i].normalize();
      }

      m_samplingPatternBuffer.createBuffer(sizeof(samplingPattern[0]) * samplingPattern.size(), GL_STATIC_DRAW);
      m_samplingPatternBuffer.setData(0, sizeof(samplingPattern[0]) * samplingPattern.size(), &samplingPattern[0]);
      m_samplingPatternBuffer.uploadData();
    }

    void IndirectLightRenderer::updateBuffer(unsigned int cacheNumber)
    {
      if(cacheNumber != m_cacheNumber)
      {
        m_cacheNumber = cacheNumber;

        GLuint resolution = pow(2.0f, ceil(log(sqrt(float(m_cacheNumber))) / log(2.0f)));

        if(resolution != m_bufferResolution)
        {
          m_bufferResolution = resolution;

          m_globalCachePositionBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
          m_globalCacheNormalBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
          m_globalCacheAreaBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));

          m_indirectLightPositionDBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
          m_indirectLightLuminousFluxDBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
          m_indirectLightPositionGBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
          m_indirectLightLuminousFluxGBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));

          m_indirectLightProxyLightCreationRenderQuad.setRenderTargets(4, 
            m_indirectLightPositionDBuffer, m_indirectLightLuminousFluxDBuffer, 
            m_indirectLightPositionGBuffer, m_indirectLightLuminousFluxGBuffer);

          m_zBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_R8, GL_RED, 1, 8));
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
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//wait until the cache data is written into the global cache image buffers

        //CPUTIMER("cpuCompute", 0)
        //GPUTIMER("gpuCompute", 1)

        const sh::RenderShader& renderShader = m_singletonManager->getService<sh::ShaderContainer>()->getRenderShader(m_indirectLightShaderHandle);

        glViewport(0, 0, m_bufferResolution, m_bufferResolution);
        renderShader.useShader();

        m_indirectLightProxyLightCreationRenderQuad.setWriteFrameBuffer();

        m_samplingPatternBuffer.bindBuffer(1);

        reflectiveShadowLightBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

        reflectiveShadowPosMaps->setTexture(0, 0);
        reflectiveShadowNormalMaps->setTexture(1, 1);
        reflectiveShadowLuminousFluxMaps->setTexture(2, 2);

        m_globalCachePositionBuffer->setTexture(3, 3);
        m_globalCacheNormalBuffer->setTexture(4, 4);
        m_globalCacheAreaBuffer->setTexture(5, 5);

        m_zBuffer->setTexture(6, 6);

        GLuint reflectiveShadowMapWidth = m_options->reflectiveShadowMapWidth;
        sh::RenderShader::setUniform(7, GL_UNSIGNED_INT, &reflectiveShadowMapWidth);

        GLuint reflectiveShadowMapNumber = reflectiveShadowPosMaps->getResolution()[2];
        sh::RenderShader::setUniform(8, GL_UNSIGNED_INT, &reflectiveShadowMapNumber);

        sh::RenderShader::setUniform(9, GL_UNSIGNED_INT, &m_bufferResolution);

        sh::RenderShader::setUniform(10, GL_UNSIGNED_INT, &m_cacheNumber);

        m_indirectLightProxyLightCreationRenderQuad.render();

        m_zBuffer->unsetTexture(6);

        m_globalCacheAreaBuffer->unsetTexture(5);
        m_globalCacheNormalBuffer->unsetTexture(4);
        m_globalCachePositionBuffer->unsetTexture(3);

        reflectiveShadowLuminousFluxMaps->unsetTexture(2);
        reflectiveShadowNormalMaps->unsetTexture(1);
        reflectiveShadowPosMaps->unsetTexture(0);

        reflectiveShadowLightBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER);
        m_samplingPatternBuffer.unBindBuffer(1);

        m_indirectLightProxyLightCreationRenderQuad.unsetWriteFrameBuffer();

        renderShader.useNoShader();

        glViewport(0, 0, m_options->width, m_options->height);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//wait until the proxy light data is written into the image buffer
      }
    }

    void IndirectLightRenderer::setBuffer(util::SharedPointer<db::Texture2D> depthBuffer)
    {
      m_gBufferSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

      GLenum state;
      do
      {
        state = glClientWaitSync(m_gBufferSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
      } while(state != GL_CONDITION_SATISFIED && state != GL_ALREADY_SIGNALED);

      util::Vector<GLubyte, 4> zeros(util::Vector<GLubyte, 4>::identity());
      m_zBuffer->clearTexture(&zeros);

      m_indirectLightIndicesRenderQuad.setRenderTargets(depthBuffer, 0);

      util::vec4ui negativeZeros(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX);
      m_indirectLightIndicesRenderQuad.clearTargets(1.0f, std::vector<util::vec4ui>(6, negativeZeros), false);

      m_indirectLightIndicesRenderQuad.setWriteFrameBuffer();

      m_globalCachePositionBuffer->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCacheNormalBuffer->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCacheAreaBuffer->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16F);

      m_zBuffer->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_R8UI);

      glDeleteSync(m_gBufferSync);
    }

    void IndirectLightRenderer::unsetBuffer() const
    {
      m_zBuffer->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_R8UI);

      m_globalCacheAreaBuffer->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16F);
      m_globalCacheNormalBuffer->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCachePositionBuffer->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);

      m_indirectLightIndicesRenderQuad.unsetWriteFrameBuffer();
    }

    void IndirectLightRenderer::setCacheAndProxyLights() const
    {
      util::vec4f clearColor(0.0f, 0.0f, 0.0f, 0.0f);
      m_indirectLightRenderQuad.clearTargets(1.0f, clearColor);

      m_indirectLightRenderQuad.setWriteFrameBuffer();

      m_globalCachePositionBuffer->bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
      m_globalCacheNormalBuffer->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);

      m_indirectLightPositionDBuffer->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightLuminousFluxDBuffer->bindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);

      m_indirectLightPositionGBuffer->bindImageTexture(4, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightLuminousFluxGBuffer->bindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);
    }

    void IndirectLightRenderer::unsetCacheAndProxyLights() const
    {
      m_indirectLightLuminousFluxGBuffer->unbindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightPositionGBuffer->unbindImageTexture(4, 0, GL_READ_ONLY, GL_RGBA32F);

      m_indirectLightLuminousFluxDBuffer->unbindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
      m_indirectLightPositionDBuffer->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);

      m_globalCacheNormalBuffer->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_globalCachePositionBuffer->unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);

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

      //std::vector<util::vec4f> cachePos(m_bufferResolution * m_bufferResolution);
      //m_globalCachePositionBuffer->getTextureData(&cachePos[0]);

      //std::vector<util::vec4f> cacheNormal(m_bufferResolution * m_bufferResolution);
      //m_globalCacheNormalBuffer->getTextureData(&cacheNormal[0]);

      return m_indirectLightMap;//m_indirectLightMap;
      //return m_TMPTEXTURE;
      //return m_indirectLightPositionDBuffer;
      //return m_globalCacheNormalBuffer;
      //return m_zBuffer;
      //return m_frameCacheIndexMap->convertToTexture2D(3);
      //return m_indirectLightLuminousFluxDBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectPositionMapDiffuse()
    {
      return m_indirectLightPositionDBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectLuminousFluxMapDiffuse()
    {
      return m_indirectLightLuminousFluxDBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectPositionMapSpecular()
    {
      return m_indirectLightPositionGBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectLuminousFluxMapSpecular()
    {
      return m_indirectLightLuminousFluxGBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getZBuffer()
    {
      return m_zBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getGlobalCachePositionMap()
    {
      return m_globalCachePositionBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getGlobalCacheNormalMap()
    {
      return m_globalCacheNormalBuffer;
    }
  }
}
