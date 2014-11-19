#include "Renderer/Pipeline/IndirectLightRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

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

      m_frameCacheIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->width * m_options->height * 6 * sizeof(util::vec4ui), 0, GL_STATIC_DRAW, nullptr);
      m_globalCacheBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 1 * sizeof(util::Cache), 0, GL_STATIC_DRAW, nullptr);
      m_zBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 1 * sizeof(GLuint), 0, GL_STATIC_DRAW, nullptr);
      m_indirectLightDataBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 1 * sizeof(IndirectLight), 0, GL_STATIC_DRAW, nullptr);

      m_indirectLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64, nullptr));

      m_indirectLightRenderQuad.setRenderTargets(1, m_indirectLightMap);
    }

    void IndirectLightRenderer::updateBuffer(unsigned int cacheNumber)
    {
      if(cacheNumber != m_cacheNumber)
      {
        m_cacheNumber = cacheNumber;
        m_globalCacheBuffer.resizeBuffer(m_cacheNumber * sizeof(util::Cache), nullptr);
        m_zBuffer.resizeBuffer(m_cacheNumber * sizeof(GLuint), nullptr);
        m_indirectLightDataBuffer.resizeBuffer(2 * m_cacheNumber * sizeof(IndirectLight), nullptr);
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
      db::ComputeShader *computeShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_indirectLightShaderHandle);

      computeShader->useShader();

      GLuint indirectLightNumberX = m_options->shadowMapWidth;
      db::RenderShader::setUniform(3, GL_UNSIGNED_INT, &indirectLightNumberX);

      GLuint indirectLightNumberY = m_options->shadowMapWidth;
      db::RenderShader::setUniform(4, GL_UNSIGNED_INT, &indirectLightNumberY);

      GLuint indirectLightNumberZ = reflectiveShadowPosMaps->getResolution()[2];
      db::RenderShader::setUniform(5, GL_UNSIGNED_INT, &indirectLightNumberZ);

      db::RenderShader::setUniform(6, GL_UNSIGNED_INT, &m_cacheNumber);
      db::RenderShader::setUniform(7, GL_UNSIGNED_INT, &m_options->unusedLightIndirectNumber);

      //std::vector<util::vec4f> luminousData(m_options->shadowMapWidth * m_options->shadowMapWidth);
      //reflectiveShadowLuminousFluxMaps->getTextureData(&luminousData[0]);

      reflectiveShadowPosMaps->setTexture(0, 0);
      reflectiveShadowNormalMaps->setTexture(1, 1);
      reflectiveShadowLuminousFluxMaps->setTexture(2, 2);

      m_globalCacheBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_zBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_indirectLightDataBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

      db::ComputeShader::dispatchComputeShader(64, 1, 1);

      m_indirectLightDataBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_zBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_globalCacheBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      reflectiveShadowLuminousFluxMaps->unsetTexture();
      reflectiveShadowNormalMaps->unsetTexture();
      reflectiveShadowPosMaps->unsetTexture();

      computeShader->useNoShader();

      //std::vector<util::Cache> caches(m_cacheNumber);
      //m_globalCacheBuffer.getData(0, sizeof(util::Cache) * m_cacheNumber, &caches[0]);

      //std::vector<GLuint> zvals(m_cacheNumber);
      //m_zBuffer.getData(0, sizeof(GLuint) * m_cacheNumber, &zvals[0]);

      //std::vector<IndirectLight> lights(2 * m_cacheNumber);
      //m_indirectLightDataBuffer.getData(0, sizeof(IndirectLight) * 2 * m_cacheNumber, &lights[0]);

      db::RenderShader *renderShader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_indirectLightInterpolationShaderHandle);

      renderShader->useShader();

      m_indirectLightRenderQuad.setWriteFrameBuffer();

      depthMap->setTexture(0, 0);
      normalMap->setTexture(1, 1);
      materialMap->setTexture(2, 2);

      m_globalCacheBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_frameCacheIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_indirectLightDataBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

      m_indirectLightRenderQuad.render();

      m_indirectLightDataBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_frameCacheIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_globalCacheBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      materialMap->unsetTexture();
      normalMap->unsetTexture();
      depthMap->unsetTexture();

      m_indirectLightRenderQuad.unsetWriteFrameBuffer();

      renderShader->useNoShader();
    }

    void IndirectLightRenderer::setBuffer() const
    {
      GLuint zeros = 0;
      m_zBuffer.clearBuffer(GL_RED, GL_R32UI, GL_UNSIGNED_INT, &zeros);
      util::vec4ui negativeZeros(~0, ~0, ~0, ~0);
      m_frameCacheIndexBuffer.clearBuffer(GL_RGBA, GL_RGBA32UI, GL_UNSIGNED_INT, &negativeZeros[0]);

      m_globalCacheBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_frameCacheIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_zBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
    }

    void IndirectLightRenderer::unsetBuffer() const
    {
      m_zBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_frameCacheIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_globalCacheBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

      //std::vector<GLuint> zBuff(m_cacheNumber);
      //m_zBuffer.getData(0, sizeof(GLuint) * m_cacheNumber, &zBuff[0]);

      //std::vector<util::vec4ui> frIndex(m_options->width * m_options->height * 6);
      //m_frameCacheIndexBuffer.getData(0, m_options->width * m_options->height * 6 * sizeof(util::vec4ui), &frIndex[0]);
      
      /*for(unsigned int i = 0; i < m_options->width * m_options->height * 6; i++)
      {
        if(frIndex[i][0] < INT32_MAX - 1 || frIndex[i][1] < INT32_MAX - 1 || frIndex[i][2] < INT32_MAX - 1 || frIndex[i][3] < INT32_MAX - 1)
        {
          util::vec4ui t = frIndex[i];
        }

        if(i >= 3072)
        {
          util::vec4ui v[6];
          for(unsigned int j = 0; j < 6; j++)
          {
            v[j] = frIndex[i + j];
          }

          util::Vector<bool, 4> bitMask;
          for(unsigned int j = 0; j < 6; j++)
          {
            bitMask = util::Vector<bool, 4>(1, 1, 1, 1);
            unsigned int index = (2 + (j / 2) * 2) % 6;

            bitMask &= util::math::vector_cast<bool>(v[j] - v[index][0]);
            bitMask &= util::math::vector_cast<bool>(v[j] - v[index][1]);
            bitMask &= util::math::vector_cast<bool>(v[j] - v[index][2]);
            bitMask &= util::math::vector_cast<bool>(v[j] - v[index][3]);

            index++;
            bitMask &= util::math::vector_cast<bool>(v[j] - v[index][0]);
            bitMask &= util::math::vector_cast<bool>(v[j] - v[index][1]);
            bitMask &= util::math::vector_cast<bool>(v[j] - v[index][2]);
            bitMask &= util::math::vector_cast<bool>(v[j] - v[index][3]);
            v[j] *= util::math::vector_cast<unsigned int>(bitMask);
          }
        }
      }
*/
      //std::vector<util::Cache> caches(m_cacheNumber);
      //m_globalCacheBuffer.getData(0, sizeof(util::Cache) * m_cacheNumber, &caches[0]);
    }

    util::SharedPointer<db::Texture2D> IndirectLightRenderer::getIndirectLightMap() const
    {
      return m_indirectLightMap;
    }
  }
}
