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

      m_indirectLightShaderHandle = singletonManager->getService<db::ShaderContainer>()->getRenderShader(m_singletonManager, db::ShaderContainer::INDIRECTLIGHTPROXYLIGHTCREATION, util::Flags<db::VertexDeclarationFlags>(8192));
      m_indirectLightInterpolationShaderHandle = singletonManager->getService<db::ShaderContainer>()->getRenderShader(singletonManager, db::ShaderContainer::INDIRECTLIGHTINTERPOLATION, util::Flags<db::VertexDeclarationFlags>(8192));

      m_bufferResolution = 64;

      m_globalCachePositionBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
      m_globalCacheNormalBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

      m_indirectLightPositionDBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
      m_indirectLightLuminousFluxDBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
      m_indirectLightPositionGBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
      m_indirectLightLuminousFluxGBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

      m_zBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_R8, GL_RED, 1, 8));

      m_frameCacheIndexMap = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->width, m_options->height, 6, GL_TEXTURE_2D_ARRAY, GL_UNSIGNED_INT, GL_RGBA32UI, GL_RGBA_INTEGER, 4, 128));
      
      m_indirectLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64));

      m_indirectLightProxyLightCreationRenderQuad.setRenderTargets(4, m_indirectLightPositionDBuffer, m_indirectLightLuminousFluxDBuffer, m_indirectLightPositionGBuffer, m_indirectLightLuminousFluxGBuffer);
      m_indirectLightRenderQuad.setRenderTargets(1, m_indirectLightMap);
    }

    void IndirectLightRenderer::updateBuffer(unsigned int cacheNumber)
    {
      if(cacheNumber != m_cacheNumber)
      {
        m_cacheNumber = cacheNumber;
        //m_globalCacheBuffer.resizeBuffer(m_cacheNumber * sizeof(util::Cache));
        //m_zBuffer.resizeBuffer(m_cacheNumber * sizeof(GLuint));
        //m_indirectLightDataBuffer.resizeBuffer(2 * m_cacheNumber * sizeof(IndirectLight));

        GLuint resolution = pow(2.0f, ceil(log(sqrt(float(m_cacheNumber))) / log(2.0f)));

        if(resolution != m_bufferResolution)
        {
          m_bufferResolution = resolution;

          m_globalCachePositionBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_globalCacheNormalBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

          m_indirectLightPositionDBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_indirectLightLuminousFluxDBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_indirectLightPositionGBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_indirectLightLuminousFluxGBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

          m_indirectLightProxyLightCreationRenderQuad.setRenderTargets(4, m_indirectLightPositionDBuffer, m_indirectLightLuminousFluxDBuffer, m_indirectLightPositionGBuffer, m_indirectLightLuminousFluxGBuffer);

          m_zBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_R8, GL_RED, 1, 8));
        }
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
        //GPUTIMER("gpuCompute", 1)

        db::RenderShader *renderShader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_indirectLightShaderHandle);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glViewport(0, 0, m_bufferResolution, m_bufferResolution);
        renderShader->useShader();

        m_indirectLightProxyLightCreationRenderQuad.setWriteFrameBuffer();

        reflectiveShadowPosMaps->setTexture(0, 0);
        reflectiveShadowNormalMaps->setTexture(1, 1);
        reflectiveShadowLuminousFluxMaps->setTexture(2, 2);

        m_globalCachePositionBuffer->setTexture(3, 3);
        m_globalCacheNormalBuffer->setTexture(4, 4);

        m_zBuffer->setTexture(5, 5);

        GLuint reflectiveShadowMapWidth = m_options->shadowMapWidth;
        db::RenderShader::setUniform(6, GL_UNSIGNED_INT, &reflectiveShadowMapWidth);

        GLuint reflectiveShadowMapNumber = reflectiveShadowPosMaps->getResolution()[2];
        db::RenderShader::setUniform(7, GL_UNSIGNED_INT, &reflectiveShadowMapNumber);

        db::RenderShader::setUniform(8, GL_UNSIGNED_INT, &m_options->unusedLightIndirectNumber);

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

        m_globalCachePositionBuffer->bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCacheNormalBuffer->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);

        m_indirectLightPositionDBuffer->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
        m_indirectLightLuminousFluxDBuffer->bindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);

        m_indirectLightPositionGBuffer->bindImageTexture(4, 0, GL_READ_ONLY, GL_RGBA32F);
        m_indirectLightLuminousFluxGBuffer->bindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);

        db::RenderShader::setUniform(8, GL_UNSIGNED_INT, &m_bufferResolution);

        m_indirectLightRenderQuad.render();

        m_indirectLightLuminousFluxGBuffer->unbindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);
        m_indirectLightPositionGBuffer->unbindImageTexture(4, 0, GL_READ_ONLY, GL_RGBA32F);

        m_indirectLightLuminousFluxDBuffer->unbindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
        m_indirectLightPositionDBuffer->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);

        m_globalCacheNormalBuffer->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCachePositionBuffer->unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);

        m_frameCacheIndexMap->unsetTexture(3);

        materialMap->unsetTexture(2);
        normalMap->unsetTexture(1);
        depthMap->unsetTexture(0);

        m_indirectLightRenderQuad.unsetWriteFrameBuffer();

        renderShader->useNoShader();
      }
    }

    void IndirectLightRenderer::setBuffer(util::SharedPointer<db::Texture2D> depthTexture)
    {
      m_indirectLightIndicesRenderQuad.setRenderTargets3D(depthTexture, 1, m_frameCacheIndexMap);

      GLubyte zeros = 0;
      m_zBuffer->clearTexture(&zeros);

      util::vec4ui negativeZeros(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX);
      m_indirectLightIndicesRenderQuad.clearTargets(1.0f, std::vector<util::vec4ui>(6, negativeZeros), false);

      m_indirectLightIndicesRenderQuad.setWriteFrameBuffer();

      m_globalCachePositionBuffer->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCacheNormalBuffer->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);

      m_zBuffer->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_R8UI);
    }

    void IndirectLightRenderer::unsetBuffer() const
    {
      m_zBuffer->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_R8UI);

      m_globalCacheNormalBuffer->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_globalCachePositionBuffer->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);

      m_indirectLightIndicesRenderQuad.unsetWriteFrameBuffer();
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectLightMap() const
    {
      return m_indirectLightMap;
      //return m_zBuffer;
      //return m_frameCacheIndexMap->convertToTexture2D(0);
      //return m_indirectLightLuminousFluxBuffer;
    }
  }
}
