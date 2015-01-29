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

      m_indirectLightShaderHandle = singletonManager->getService<db::ShaderContainer>()->getComputeShader(m_singletonManager, db::ShaderContainer::INDIRECTLIGHT);
      m_indirectLightInterpolationShaderHandle = singletonManager->getService<db::ShaderContainer>()->getRenderShader(singletonManager, db::ShaderContainer::INDIRECTLIGHTINTERPOLATION, util::Flags<db::VertexDeclarationFlags>(8192));

      m_bufferResolution = 64;

      //m_globalCacheBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Cache) * m_options->globalCacheBlockSize, 1 * sizeof(util::Cache), GL_STATIC_DRAW, nullptr);
      //m_zBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_options->globalCacheBlockSize, 1 * sizeof(util::Cache), GL_STATIC_DRAW, nullptr);
      //m_indirectLightDataBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(IndirectLight) * m_options->globalCacheBlockSize * 2, 1 * sizeof(util::Cache), GL_STATIC_DRAW, nullptr);

      m_globalCachePositionBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
      m_globalCacheNormalBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

      m_indirectLightPositionBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
      m_indirectLightLuminousFluxBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

      m_zBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_R8, GL_RED, 1, 8));

      m_frameCacheIndexMap = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->width, m_options->height, 6, GL_TEXTURE_2D_ARRAY, GL_UNSIGNED_INT, GL_RGBA32UI, GL_RGBA_INTEGER, 4, 128));
      
      m_indirectLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64));

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

          m_indirectLightPositionBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
          m_indirectLightLuminousFluxBuffer = util::SharedPointer<db::Texture2D>(new db::Texture2D(2 * m_bufferResolution, m_bufferResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));

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
        GPUTIMER("gpuCompute", 1)

        db::ComputeShader *computeShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_indirectLightShaderHandle);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        computeShader->useShader();

        reflectiveShadowPosMaps->setTexture(0, 0);
        reflectiveShadowNormalMaps->setTexture(1, 1);
        reflectiveShadowLuminousFluxMaps->setTexture(2, 2);

        m_globalCachePositionBuffer->bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCacheNormalBuffer->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);

        m_indirectLightPositionBuffer->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
        m_indirectLightLuminousFluxBuffer->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA32F);

        m_zBuffer->bindImageTexture(4, 0, GL_READ_ONLY, GL_R8UI);

        GLuint indirectLightNumberXY = m_options->shadowMapWidth;
        db::RenderShader::setUniform(8, GL_UNSIGNED_INT, &indirectLightNumberXY);

        GLuint indirectLightNumberZ = reflectiveShadowPosMaps->getResolution()[2];
        db::RenderShader::setUniform(9, GL_UNSIGNED_INT, &indirectLightNumberZ);

        db::RenderShader::setUniform(10, GL_UNSIGNED_INT, &m_cacheNumber);

        db::RenderShader::setUniform(11, GL_UNSIGNED_INT, &m_options->unusedLightIndirectNumber);

        db::RenderShader::setUniform(12, GL_UNSIGNED_INT, &m_bufferResolution);

        db::ComputeShader::dispatchComputeShader(128, 1, 1);

        m_zBuffer->unbindImageTexture(4, 0, GL_READ_ONLY, GL_R8UI);

        m_indirectLightLuminousFluxBuffer->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA32F);
        m_indirectLightPositionBuffer->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);

        m_globalCacheNormalBuffer->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCachePositionBuffer->unbindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);

        reflectiveShadowLuminousFluxMaps->unsetTexture(2);
        reflectiveShadowNormalMaps->unsetTexture(1);
        reflectiveShadowPosMaps->unsetTexture(0);

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

        m_globalCachePositionBuffer->bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA32F);
        m_globalCacheNormalBuffer->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);

        m_indirectLightPositionBuffer->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
        m_indirectLightLuminousFluxBuffer->bindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);

        db::RenderShader::setUniform(8, GL_UNSIGNED_INT, &m_bufferResolution);

        m_indirectLightRenderQuad.render();

        m_indirectLightLuminousFluxBuffer->unbindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
        m_indirectLightPositionBuffer->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);

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
