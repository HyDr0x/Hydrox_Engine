#include "Renderer/Pipeline/IndirectSpecularReflections.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/RandomSequenceGenerator.h>
#include <Utilities/Timer/Timer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    IndirectSpecularReflections::IndirectSpecularReflections()
    {
      glGenVertexArrays(1, &m_pointVAO);
    }

    IndirectSpecularReflections::~IndirectSpecularReflections()
    {
      glDeleteVertexArrays(1, &m_pointVAO);
    }

    void IndirectSpecularReflections::initialize(util::SingletonManager *singletonManager)
    {
      glBindVertexArray(m_pointVAO);
      glVertexAttribFormat(sh::RenderShader::POSITION, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::POSITION, 0);
      glVertexAttribFormat(sh::RenderShader::NORMAL, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::NORMAL, 1);
      glBindVertexArray(0);

      m_options = singletonManager->getService<RenderOptions>();

      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_proxyLightOffsetShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::INDIRECTSHADOWMAPCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION) | sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::NORMAL));
      m_proxyLightCreationShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::VISIBILITYMAPCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));

      m_specularProxyLightOffsets = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));
      m_specularProxyIndices = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));

      m_proxyLightBoundingBoxBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::vec3f), sizeof(util::vec3f), GL_STATIC_DRAW);
      m_proxyLightBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), 2 * m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_cacheBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
    }

    void IndirectSpecularReflections::updateBuffer(unsigned int reflectiveShadowMapNumber)
    {
      if(reflectiveShadowMapNumber != m_reflectiveShadowMapNumber)
      {
        m_reflectiveShadowMapNumber = reflectiveShadowMapNumber;
      }
    }

    void IndirectSpecularReflections::generateReflectionCaches(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      const GPUBuffer& samplingPatternBuffer)
    {
      createProxyLightOffsetBuffer(gBufferDepthMap, gBufferNormalMap, indirectLightPositions, samplingPatternBuffer);

      createProxyLights(gBufferDepthMap, gBufferNormalMap, indirectLightPositions, samplingPatternBuffer);

      createProxyLightIndices();
      
    }

    void IndirectSpecularReflections::createProxyLightOffsetBuffer(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      const GPUBuffer& samplingPatternBuffer)
    {
      //glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
      //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      //const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_proxyLightOffsetShaderHandle);
      //shader.useShader();

      //glViewport(0, 0, m_options->indirectShadowMapWidth, m_options->indirectShadowMapWidth);

      //sh::RenderShader::setUniform(0, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);

      //glBindVertexArray(m_pointVAO);
      //glEnableVertexAttribArray(sh::RenderShader::POSITION);
      //glEnableVertexAttribArray(sh::RenderShader::NORMAL);

      //samplingPatternBuffer.bindBuffer(1);

      //glPointSize(6.0f);

      //glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      //for(unsigned int i = 0; i < m_reflectiveShadowMapNumber; i++)
      //{
      //  m_indirectLightShadowMapsQuad.setDepthLayer(i);
      //  m_indirectLightShadowMapsQuad.clearTargets(1.0f, he::util::vec4f::identity());
      //  m_indirectLightShadowMapsQuad.setWriteFrameBuffer();

      //  shadowPosMaps->bindImageTexture(0, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);
      //  shadowNormalMaps->bindImageTexture(1, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);

      //  globalCachePositionBuffer.bindVertexbuffer(0, 0, sizeof(util::vec4f));
      //  globalCacheNormalBuffer.bindVertexbuffer(1, 0, sizeof(util::vec4f));

      //  glDrawArraysInstanced(GL_POINTS, 0, cacheNumber, m_options->giLightSampleNumber);

      //  globalCacheNormalBuffer.unbindVertexBuffer(1);
      //  globalCachePositionBuffer.unbindVertexBuffer(0);

      //  adaptiveSamplingPositionBuffer.bindVertexbuffer(0, 0, sizeof(util::vec4f));
      //  adaptiveSamplingNormalBuffer.bindVertexbuffer(1, 0, sizeof(util::vec4f));

      //  commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);
      //  glMultiDrawArraysIndirect(GL_POINTS, nullptr, 1, commandBuffer.getBufferSize());//buffer size is okay, because we only have one command struct in it
      //  commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);

      //  adaptiveSamplingNormalBuffer.unbindVertexBuffer(1);
      //  adaptiveSamplingPositionBuffer.unbindVertexBuffer(0);

      //  shadowNormalMaps->unbindImageTexture(1, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);
      //  shadowPosMaps->unbindImageTexture(0, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);

      //  m_indirectLightShadowMapsQuad.unsetWriteFrameBuffer();
      //}

      //glPointSize(1.0f);

      //samplingPatternBuffer.unbindBuffer(1);
      //
      //glDisableVertexAttribArray(sh::RenderShader::NORMAL);
      //glDisableVertexAttribArray(sh::RenderShader::POSITION);
      //glBindVertexArray(0);

      //shader.useNoShader();

      //glViewport(0, 0, m_options->width, m_options->height);
    }

    void IndirectSpecularReflections::createProxyLights(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      const GPUBuffer& samplingPatternBuffer)
    {
      /*glViewport(0, 0, 0.5f * m_options->width, 0.5f * m_options->height);

      const sh::RenderShader& indirectShadowMapCreationShader = m_shaderContainer->getRenderShader(m_indirectShadowMapCreationShaderHandle);
      indirectShadowMapCreationShader.useShader();

      sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);
      sh::RenderShader::setUniform(3, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);

      m_indirectShadowMapQuad.setRenderTargets(1, m_indirectShadowMap0);
      m_indirectShadowMapQuad.setWriteFrameBuffer();

      indirectLightPositions->bindImageTexture(0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
      indirectLightNormals->bindImageTexture(1, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

      gBufferDepthMap->setTexture(0, 0);
      m_indirectLightShadowMaps->setTexture(1, 1);

      samplingPatternBuffer.bindBuffer(1);

      m_indirectShadowMapQuad.render();

      samplingPatternBuffer.unbindBuffer(1);

      m_indirectLightShadowMaps->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      indirectLightNormals->unbindImageTexture(1, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
      indirectLightPositions->unbindImageTexture(0, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

      m_indirectShadowMapQuad.unsetWriteFrameBuffer();
      indirectShadowMapCreationShader.useNoShader();

      glViewport(0, 0, m_options->width, m_options->height);*/
    }

    void IndirectSpecularReflections::createProxyLightIndices()
    {

    }

    const GPUBuffer& IndirectSpecularReflections::getSpecularProxyLights() const
    {
      return m_proxyLightBuffer;
    }

    const GPUBuffer& IndirectSpecularReflections::getSpecularCacheMap() const
    {
      return m_cacheBuffer;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getSpecularProxyIndexMap() const
    {
      return m_specularProxyIndices;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getSpecularProxyOffsets() const
    {
      return m_specularProxyLightOffsets;
    }
  }
}
