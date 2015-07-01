#include "Renderer/Pipeline/IndirectShadowsCreation.h"

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
    IndirectShadowsCreation::IndirectShadowsCreation() : m_reflectiveShadowMapNumber(0)
    {
      glGenVertexArrays(1, &m_pointVAO);
    }

    IndirectShadowsCreation::~IndirectShadowsCreation()
    {
      glDeleteVertexArrays(1, &m_pointVAO);
    }

    void IndirectShadowsCreation::initialize(util::SingletonManager *singletonManager)
    {
      glBindVertexArray(m_pointVAO);
      glVertexAttribFormat(sh::RenderShader::POSITION, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::POSITION, 0);
      glVertexAttribFormat(sh::RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::SPECIAL0, 1);
      glBindVertexArray(0);

      m_options = singletonManager->getService<RenderOptions>();

      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_indirectShadowMapsShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::INDIRECTSHADOWMAPCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION) | sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL0));
      m_indirectShadowMapCreationShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::VISIBILITYMAPCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_pushShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::PUSHSHADER, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_viewMatrixCreationHandle = sh::ShaderContainer::INDIRECTSHADOWVIEWMATRIXCREATION;

      m_indirectShadowMap0 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));
      m_indirectShadowMap1 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));

      m_geometryBackprojectionDepthMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
    }

    void IndirectShadowsCreation::updateBuffer(unsigned int reflectiveShadowMapNumber)
    {
      if(reflectiveShadowMapNumber != m_reflectiveShadowMapNumber)
      {
        m_reflectiveShadowMapNumber = reflectiveShadowMapNumber;

        m_indirectLightShadowMaps = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->indirectShadowMapWidth, m_options->indirectShadowMapWidth, m_reflectiveShadowMapNumber, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));

        m_indirectLightPushPullShadowMaps.resize(m_options->pushPullPyramideSize);

        for(unsigned int i = 0; i < m_options->pushPullPyramideSize; i++)
        {
          unsigned int divisor = std::pow(2, float(i + 1));
          m_indirectLightPushPullShadowMaps[i] = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->indirectShadowMapWidth / divisor, m_options->indirectShadowMapWidth / divisor, m_reflectiveShadowMapNumber, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
        }
        
        m_indirectLightShadowMapsQuad.setRenderTargets3D(m_indirectLightShadowMaps, 0, 0);

        m_geometryBackprojectionMap = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->width, m_options->height, m_reflectiveShadowMapNumber, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
        m_backprojectionRenderQuad.setRenderTargets3D(m_geometryBackprojectionDepthMap, 1, m_geometryBackprojectionMap);
      }
    }

    void IndirectShadowsCreation::generateImperfectShadowMap(
      util::SharedPointer<db::Texture3D> shadowPosMaps, 
      util::SharedPointer<db::Texture3D> shadowNormalMaps, 
      const TBO& globalCachePositionBuffer,
      const TBO& globalCacheAreaBuffer,
      const UBO& samplingPatternBuffer, 
      const GPUImmutableBuffer& reflectiveShadowLightsBuffer,
      unsigned int cacheNumber)
    {
      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      //glDisable(GL_DEPTH_TEST);
      //glDepthMask(GL_FALSE);
      //glDepthFunc(GL_LEQUAL);

      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_indirectShadowMapsShaderHandle);
      shader.useShader();

      glViewport(0, 0, m_options->indirectShadowMapWidth, m_options->indirectShadowMapWidth);

      sh::RenderShader::setUniform(0, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);

      glBindVertexArray(m_pointVAO);
      glEnableVertexAttribArray(sh::RenderShader::POSITION);
      glEnableVertexAttribArray(sh::RenderShader::SPECIAL0);

      globalCachePositionBuffer.bindVertexbuffer(0, 0, sizeof(util::vec4f));
      globalCacheAreaBuffer.bindVertexbuffer(1, 0, 2);

      samplingPatternBuffer.bindBuffer(1);
      reflectiveShadowLightsBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      //glDisable(GL_CULL_FACE);
      glPointSize(6.0f);

      for(unsigned int i = 0; i < m_reflectiveShadowMapNumber; i++)
      {
        m_indirectLightShadowMapsQuad.setDepthLayer(i);
        m_indirectLightShadowMapsQuad.clearTargets(1.0f, he::util::vec4f::identity());
        //m_indirectLightShadowMapsQuad.clearTargets(1.0f, util::vec4f(1.0f), std::vector<unsigned int>(1, i), true);
        //m_indirectLightShadowMapsQuad.setWriteFrameBuffer(std::vector<GLuint>(1, i));
        m_indirectLightShadowMapsQuad.setWriteFrameBuffer();

        sh::RenderShader::setUniform(1, GL_UNSIGNED_INT, &i);

        shadowPosMaps->bindImageTexture(0, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);
        shadowNormalMaps->bindImageTexture(1, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);
        m_geometryBackprojectionMap->bindImageTexture(2, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);

        glDrawArraysInstanced(GL_POINTS, 0, cacheNumber, m_options->giLightSampleNumber);

        m_geometryBackprojectionMap->unbindImageTexture(2, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);
        shadowNormalMaps->unbindImageTexture(1, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);
        shadowPosMaps->unbindImageTexture(0, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);

        m_indirectLightShadowMapsQuad.unsetWriteFrameBuffer();
      }

      glPointSize(1.0f);
      //glEnable(GL_CULL_FACE);

      reflectiveShadowLightsBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      samplingPatternBuffer.unbindBuffer(1);
      globalCacheAreaBuffer.unbindVertexBuffer(1);
      globalCachePositionBuffer.unbindVertexBuffer(0);
      glDisableVertexAttribArray(sh::RenderShader::SPECIAL0);
      glDisableVertexAttribArray(sh::RenderShader::POSITION);
      glBindVertexArray(0);

      shader.useNoShader();

      //glDepthFunc(GL_LESS);
      //glDepthMask(GL_TRUE);
      //glEnable(GL_DEPTH_TEST);

      glViewport(0, 0, m_options->width, m_options->height);
    }

    void IndirectShadowsCreation::generateIndirectLightsShadowMap(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      util::SharedPointer<db::Texture3D> indirectLightNormals,
      const UBO& samplingPatternBuffer)
    {
      pullPush();

      generateShadowMap(gBufferDepthMap, indirectLightPositions, indirectLightNormals, samplingPatternBuffer);

      blurShadowMap(gBufferDepthMap, gBufferNormalMap);
    }

    void IndirectShadowsCreation::generateShadowMap(util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      util::SharedPointer<db::Texture3D> indirectLightNormals,
      const UBO& samplingPatternBuffer)
    {
      const sh::RenderShader& indirectShadowMapCreationShader = m_shaderContainer->getRenderShader(m_indirectShadowMapCreationShaderHandle);
      indirectShadowMapCreationShader.useShader();

      sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);
      sh::RenderShader::setUniform(3, GL_UNSIGNED_INT, &m_indirectLightShadowMaps->getResolution()[2]);

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
    }

    void IndirectShadowsCreation::blurShadowMap(util::SharedPointer<db::Texture2D> gBufferDepthMap, util::SharedPointer<db::Texture2D> gBufferNormalMap)
    {
      const sh::ComputeShader& indirectShadowMapBlurShader = m_shaderContainer->getComputeShader(sh::ShaderContainer::INDIRECTSHADOWMAPBLUR);
      indirectShadowMapBlurShader.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA16F);

      {
        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT_VEC2, &gBufferDepthMap->getResolution()[0]);

        util::vec2i samplingDirection(1, 0);
        sh::ComputeShader::setUniform(2, GL_INT_VEC2, &samplingDirection[0]);

        m_indirectShadowMap0->bindImageTexture(1, 0, GL_READ_ONLY, GL_R16F);
        m_indirectShadowMap1->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16F);

        sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        m_indirectShadowMap1->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16F);
        m_indirectShadowMap0->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R16F);
      }

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      {
        util::vec2ui rotatedResolution = util::vec2ui(gBufferDepthMap->getResolution()[1], gBufferDepthMap->getResolution()[0]);
        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT_VEC2, &rotatedResolution[0]);

        util::vec2i samplingDirection(0, 1);
        sh::ComputeShader::setUniform(2, GL_INT_VEC2, &samplingDirection[0]);

        m_indirectShadowMap1->bindImageTexture(1, 0, GL_READ_ONLY, GL_R16F);
        m_indirectShadowMap0->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16F);

        sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        m_indirectShadowMap0->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16F);
        m_indirectShadowMap1->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R16F);
      }

      gBufferNormalMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA16F);
      gBufferDepthMap->unsetTexture(0);

      indirectShadowMapBlurShader.useNoShader();
    }

    void IndirectShadowsCreation::pullPush()
    {
      const sh::ComputeShader& pull = m_shaderContainer->getComputeShader(sh::ShaderContainer::PULL);
      pull.useShader();

      unsigned int textureSize = m_options->indirectShadowMapWidth;
      for(unsigned int j = 0; j < m_reflectiveShadowMapNumber; j++)
      {
        for(unsigned int i = 0; i < m_options->pushPullPyramideSize; i++)
        {
          glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

          m_indirectLightPushPullShadowMaps[i]->bindImageTexture(0, 0, GL_FALSE, j, GL_WRITE_ONLY, GL_R32F);

          if(i == 0)
          {
            m_indirectLightShadowMaps->setTexture(0, 0);
          }
          else
          {
            m_indirectLightPushPullShadowMaps[i - 1]->setTexture(0, 0);
          }

          textureSize /= 2;

          sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT, &textureSize);
          sh::ComputeShader::setUniform(2, GL_UNSIGNED_INT, &j);

          sh::ComputeShader::dispatchComputeShader(128, 1, 1);

          m_indirectLightPushPullShadowMaps[i]->unbindImageTexture(0, 0, GL_FALSE, j, GL_WRITE_ONLY, GL_R32F);

          if(i == 0)
          {
            m_indirectLightShadowMaps->unsetTexture(0);
          }
          else
          {
            m_indirectLightPushPullShadowMaps[i - 1]->unsetTexture(0);
          }
        }
      }

      pull.useNoShader();

      /*const sh::ComputeShader& push = m_shaderContainer->getComputeShader(sh::ShaderContainer::PUSH);
      push.useShader();

      for(unsigned int i = m_options->pushPullPyramideSize; i > 0; i--)
      {
        sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &textureSize);
        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT, &i);

        sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        textureSize *= 2;

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      }
      push.useNoShader();*/

      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

      glViewport(0, 0, m_options->indirectShadowMapWidth, m_options->indirectShadowMapWidth);

      const sh::RenderShader& push = m_shaderContainer->getRenderShader(m_pushShaderHandle);
      push.useShader();

      m_indirectLightShadowMapsQuad.setWriteFrameBuffer();

      for(unsigned int j = 0; j < m_reflectiveShadowMapNumber; j++)
      {
        for(int i = m_options->pushPullPyramideSize - 1; i >= 0; i--)
        {
          sh::RenderShader::setUniform(1, GL_UNSIGNED_INT, &j);

          m_indirectLightPushPullShadowMaps[i]->setTexture(0, 0);
          m_indirectLightShadowMapsQuad.render();
          m_indirectLightPushPullShadowMaps[i]->unsetTexture(0);
        }
      }

      m_indirectLightShadowMapsQuad.unsetWriteFrameBuffer();

      push.useNoShader();

      glViewport(0, 0, m_options->width, m_options->height);
    }

    void IndirectShadowsCreation::viewMatrixCreation(util::SharedPointer<db::Texture3D> shadowPosMaps,
                                                     util::SharedPointer<db::Texture3D> shadowNormalMaps)
    {
      const sh::ComputeShader& viewMatrixCreationShader = m_shaderContainer->getComputeShader(m_viewMatrixCreationHandle);
      viewMatrixCreationShader.useShader();

      shadowPosMaps->setTexture(0, 0);
      shadowNormalMaps->setTexture(1, 1);

      sh::ComputeShader::dispatchComputeShader(m_reflectiveShadowMapNumber, 1, 1);

      viewMatrixCreationShader.useNoShader();
    }

    void IndirectShadowsCreation::setBackprojectionMap()
    {
      m_backprojectionRenderQuad.clearTargets(1.0f, util::vec4f(0.0f));
      m_backprojectionRenderQuad.setWriteFrameBuffer();
    }

    void IndirectShadowsCreation::unsetBackprojectionMap()
    {
      m_backprojectionRenderQuad.unsetWriteFrameBuffer();
    }

    util::SharedPointer<db::Texture3D> IndirectShadowsCreation::getIndirectLightsShadowMaps() const
    {
      return m_indirectLightShadowMaps;
    }

    util::SharedPointer<db::Texture2D> IndirectShadowsCreation::getIndirectShadowMap() const
    {
      return m_indirectShadowMap0;
    }

    std::vector<util::SharedPointer<db::Texture3D>> IndirectShadowsCreation::getIndirectPsuhPullShadowMap() const
    {
      return m_indirectLightPushPullShadowMaps;
    }

    util::SharedPointer<db::Texture3D> IndirectShadowsCreation::getBackprojectionMaps() const
    {
      return m_geometryBackprojectionMap;
    }
  }
}
