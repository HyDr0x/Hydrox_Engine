#include "Renderer/Pipeline/IndirectLightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Timer/Timer.h>

#include <DataBase/ComputeShader.h>
#include <DataBase/ResourceManager.hpp>
#include <DataBase/ShaderContainer.h>

#include "Renderer/Pipeline/RenderOptions.h"

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

      m_indirectLightShaderHandle = singletonManager->getService<db::ShaderContainer>()->getRenderShader(m_singletonManager, db::ShaderContainer::INDIRECTLIGHTPROXYLIGHTCREATION, util::Flags<VertexElements>(8192));
      m_bufferResolution = 0;

      m_depthBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
      m_indirectLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64));

      m_indirectLightRenderQuad.setRenderTargets(m_depthBuffer, 1, m_indirectLightMap);
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

          m_globalCachePositionBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_globalCacheNormalBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

          m_indirectLightPositionDBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_indirectLightLuminousFluxDBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_indirectLightPositionGBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_indirectLightLuminousFluxGBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

          m_indirectLightProxyLightCreationRenderQuad.setRenderTargets(4, m_indirectLightPositionDBuffer, m_indirectLightLuminousFluxDBuffer, m_indirectLightPositionGBuffer, m_indirectLightLuminousFluxGBuffer);

          m_zBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_R8, GL_RED, 1, 8));
        }
      }
    }

    void IndirectLightRenderer::calculateIndirectLight(
      util::SharedPointer<db::Texture3D> reflectiveShadowPosMaps,
      util::SharedPointer<db::Texture3D> reflectiveShadowNormalMaps,
      util::SharedPointer<db::Texture3D> reflectiveShadowLuminousFluxMaps) const
    {
      {
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//wait until the cache data is written into the global cache image buffers

        //CPUTIMER("cpuCompute", 0)
        //GPUTIMER("gpuCompute", 1)

        db::RenderShader *renderShader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_indirectLightShaderHandle);

        glViewport(0, 0, m_bufferResolution, m_bufferResolution);
        renderShader->useShader();

        m_indirectLightProxyLightCreationRenderQuad.setWriteFrameBuffer();

        reflectiveShadowPosMaps->setTexture(0, 0);
        reflectiveShadowNormalMaps->setTexture(1, 1);
        reflectiveShadowLuminousFluxMaps->setTexture(2, 2);

        m_globalCachePositionBuffer->setTexture(3, 3);
        m_globalCacheNormalBuffer->setTexture(4, 4);

        m_zBuffer->setTexture(5, 5);

        GLuint reflectiveShadowMapWidth = m_options->reflectiveShadowMapWidth;
        db::RenderShader::setUniform(6, GL_UNSIGNED_INT, &reflectiveShadowMapWidth);

        GLuint reflectiveShadowMapNumber = reflectiveShadowPosMaps->getResolution()[2];
        db::RenderShader::setUniform(7, GL_UNSIGNED_INT, &reflectiveShadowMapNumber);

        m_indirectLightProxyLightCreationRenderQuad.render();

        m_zBuffer->unsetTexture(5);

        m_globalCacheNormalBuffer->unsetTexture(4);
        m_globalCachePositionBuffer->unsetTexture(3);

        reflectiveShadowLuminousFluxMaps->unsetTexture(2);
        reflectiveShadowNormalMaps->unsetTexture(1);
        reflectiveShadowPosMaps->unsetTexture(0);

        m_indirectLightProxyLightCreationRenderQuad.unsetWriteFrameBuffer();

        renderShader->useNoShader();

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

      GLubyte zeros = 0;
      m_zBuffer->clearTexture(&zeros);

      m_indirectLightIndicesRenderQuad.setRenderTargets(depthBuffer, 0);

      util::vec4ui negativeZeros(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX);
      m_indirectLightIndicesRenderQuad.clearTargets(1.0f, std::vector<util::vec4ui>(6, negativeZeros), false);

      m_indirectLightIndicesRenderQuad.setWriteFrameBuffer();

      m_globalCachePositionBuffer->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCacheNormalBuffer->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);

      m_zBuffer->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_R8UI);

      glDeleteSync(m_gBufferSync);
    }

    void IndirectLightRenderer::unsetBuffer() const
    {
      m_zBuffer->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_R8UI);

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

      return m_indirectLightMap;
      //return m_TMPTEXTURE;
      //return m_indirectLightPositionDBuffer;
      //return m_globalCacheNormalBuffer;
      //return m_zBuffer;
      //return m_frameCacheIndexMap->convertToTexture2D(3);
      //return m_indirectLightLuminousFluxDBuffer;
    }
  }
}
