#include "Renderer/Pipeline/IndirectShadowsCreation.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/RandomSequenceGenerator.h>
#include <Utilities/Timer/Timer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

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
      glVertexAttribFormat(sh::RenderShader::NORMAL, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::NORMAL, 1);
      glBindVertexArray(0);

      m_options = singletonManager->getService<RenderOptions>();

      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_indirectShadowMapsShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::INDIRECTSHADOWMAPCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION) | sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::NORMAL));
      m_indirectShadowMapCreationShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::VISIBILITYMAPCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_pullShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::PULLSHADER, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_pushShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::PUSHSHADER, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_viewMatrixCreationHandle = sh::ShaderContainer::INDIRECTSHADOWVIEWMATRIXCREATION;

      m_indirectShadowMap0 = util::SharedPointer<db::Texture2D>(new db::Texture2D(0.5f * m_options->width, 0.5f * m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));
      m_indirectShadowMap1 = util::SharedPointer<db::Texture2D>(new db::Texture2D(0.5f * m_options->width, 0.5f * m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));
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
          m_indirectLightPushPullShadowMaps[i] = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->indirectShadowMapWidth / divisor, m_options->indirectShadowMapWidth / divisor, m_reflectiveShadowMapNumber, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
        }
        
        m_indirectLightShadowMapsQuad.setRenderTargets3D(m_indirectLightShadowMaps, 0, 0);

        m_geometryBackprojectionDepthMap = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->backProjectionWidth, m_options->backProjectionWidth, m_reflectiveShadowMapNumber, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
        m_geometryBackprojectionPositionMap = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->backProjectionWidth, m_options->backProjectionWidth, m_reflectiveShadowMapNumber, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
        m_geometryBackprojectionNormalMap = util::SharedPointer<db::Texture3D>(new db::Texture3D(m_options->backProjectionWidth, m_options->backProjectionWidth, m_reflectiveShadowMapNumber, GL_TEXTURE_2D_ARRAY, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 128));
        m_backprojectionRenderQuad.setRenderTargets3D(m_geometryBackprojectionDepthMap, 0, 2, m_geometryBackprojectionPositionMap, m_geometryBackprojectionNormalMap);
      }
    }

    void IndirectShadowsCreation::generateImperfectShadowMap(
      util::SharedPointer<db::Texture3D> shadowPosMaps, 
      util::SharedPointer<db::Texture3D> shadowNormalMaps, 
      const TBO& globalCachePositionBuffer,
      const TBO& globalCacheNormalBuffer,
      const TBO& adaptiveSamplingPositionBuffer,
      const TBO& adaptiveSamplingNormalBuffer,
      const UBO& samplingPatternBuffer,
      const GPUBuffer& commandBuffer,
      unsigned int cacheNumber)
    {
      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_indirectShadowMapsShaderHandle);
      shader.useShader();

      glViewport(0, 0, m_options->indirectShadowMapWidth, m_options->indirectShadowMapWidth);

      sh::RenderShader::setUniform(0, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);

      glBindVertexArray(m_pointVAO);
      glEnableVertexAttribArray(sh::RenderShader::POSITION);
      glEnableVertexAttribArray(sh::RenderShader::NORMAL);

      samplingPatternBuffer.bindBuffer(1);

      glPointSize(6.0f);

      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      for(unsigned int i = 0; i < m_reflectiveShadowMapNumber; i++)
      {
        m_indirectLightShadowMapsQuad.setDepthLayer(i);
        m_indirectLightShadowMapsQuad.clearTargets(1.0f, he::util::vec4f::identity());
        m_indirectLightShadowMapsQuad.setWriteFrameBuffer();

        shadowPosMaps->bindImageTexture(0, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);
        shadowNormalMaps->bindImageTexture(1, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);

        globalCachePositionBuffer.bindVertexbuffer(0, 0, sizeof(util::vec4f));
        globalCacheNormalBuffer.bindVertexbuffer(1, 0, sizeof(util::vec4f));

        glDrawArraysInstanced(GL_POINTS, 0, cacheNumber, m_options->giLightSampleNumber);

        globalCacheNormalBuffer.unbindVertexBuffer(1);
        globalCachePositionBuffer.unbindVertexBuffer(0);

        adaptiveSamplingPositionBuffer.bindVertexbuffer(0, 0, sizeof(util::vec4f));
        adaptiveSamplingNormalBuffer.bindVertexbuffer(1, 0, sizeof(util::vec4f));

        commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);
        glMultiDrawArraysIndirect(GL_POINTS, nullptr, 1, commandBuffer.getBufferSize());//buffer size is okay, because we only have one command struct in it
        commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);

        adaptiveSamplingNormalBuffer.unbindVertexBuffer(1);
        adaptiveSamplingPositionBuffer.unbindVertexBuffer(0);

        shadowNormalMaps->unbindImageTexture(1, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);
        shadowPosMaps->unbindImageTexture(0, 0, GL_FALSE, i, GL_READ_ONLY, GL_RGBA32F);

        m_indirectLightShadowMapsQuad.unsetWriteFrameBuffer();
      }

      glPointSize(1.0f);

      samplingPatternBuffer.unbindBuffer(1);
      
      glDisableVertexAttribArray(sh::RenderShader::NORMAL);
      glDisableVertexAttribArray(sh::RenderShader::POSITION);
      glBindVertexArray(0);

      shader.useNoShader();

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
      glViewport(0, 0, 0.5f * m_options->width, 0.5f * m_options->height);

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

      glViewport(0, 0, m_options->width, m_options->height);
    }

    void IndirectShadowsCreation::blurShadowMap(util::SharedPointer<db::Texture2D> gBufferDepthMap, util::SharedPointer<db::Texture2D> gBufferNormalMap)
    {
      const sh::ComputeShader& indirectShadowMapBlurShader = m_shaderContainer->getComputeShader(sh::ShaderContainer::INDIRECTSHADOWMAPBLUR);
      indirectShadowMapBlurShader.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_RGBA16F);

      util::vec2ui frameBufferResoltion = util::vec2ui(0.5f * m_options->width, 0.5f * m_options->height);

      {
        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT_VEC2, &frameBufferResoltion[0]);

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
        util::vec2ui rotatedResolution = util::vec2ui(frameBufferResoltion[1], frameBufferResoltion[0]);
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
      const sh::RenderShader& pull = m_shaderContainer->getRenderShader(m_pullShaderHandle);
      pull.useShader();

      unsigned int textureSize = m_options->indirectShadowMapWidth;
      for(unsigned int j = 0; j < m_reflectiveShadowMapNumber; j++)
      {
        for(unsigned int i = 0; i < m_options->pushPullPyramideSize; i++)
        {
          textureSize /= 2;
          glViewport(0, 0, textureSize, textureSize);

          GLfloat clearDepth = 1.0f;
          m_indirectLightPushPullShadowMaps[i]->clearTexture(&clearDepth);
          m_indirectLightShadowMapsQuad.setRenderTargets3D(m_indirectLightPushPullShadowMaps[i], j, 0);
          m_indirectLightShadowMapsQuad.setWriteFrameBuffer();

          if(i == 0)
          {
            m_indirectLightShadowMaps->setTexture(0, 0);
          }
          else
          {
            m_indirectLightPushPullShadowMaps[i - 1]->setTexture(0, 0);
          }

          sh::RenderShader::setUniform(1, GL_UNSIGNED_INT, &j);
          sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &i);

          m_indirectLightShadowMapsQuad.render();

          if(i == 0)
          {
            m_indirectLightShadowMaps->unsetTexture(0);
          }
          else
          {
            m_indirectLightPushPullShadowMaps[i - 1]->unsetTexture(0);
          }

          m_indirectLightShadowMapsQuad.unsetWriteFrameBuffer();
        }
      }

      pull.useNoShader();

      const sh::RenderShader& push = m_shaderContainer->getRenderShader(m_pushShaderHandle);
      push.useShader();

      for(unsigned int j = 0; j < m_reflectiveShadowMapNumber; j++)
      {
        for(int i = m_options->pushPullPyramideSize - 1; i >= 0; i--)
        {
          float textureDivisor = 1.0f / std::pow(2.0f, i);
          glViewport(0, 0, m_options->indirectShadowMapWidth * textureDivisor, m_options->indirectShadowMapWidth * textureDivisor);
          if(i > 0)
          {
            m_indirectLightShadowMapsQuad.setRenderTargets3D(m_indirectLightPushPullShadowMaps[i - 1], j, 0);
          }
          else
          {
            m_indirectLightShadowMapsQuad.setRenderTargets3D(m_indirectLightShadowMaps, j, 0);
          }

          m_indirectLightShadowMapsQuad.setWriteFrameBuffer();

          sh::RenderShader::setUniform(1, GL_UNSIGNED_INT, &j);

          m_indirectLightPushPullShadowMaps[i]->setTexture(0, 0);
          m_indirectLightShadowMapsQuad.render();
          m_indirectLightPushPullShadowMaps[i]->unsetTexture(0);

          m_indirectLightShadowMapsQuad.unsetWriteFrameBuffer();
        }
      }

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

    void IndirectShadowsCreation::setBackprojectionMap(unsigned int lightIndex)
    {
      glViewport(0, 0, m_options->backProjectionWidth, m_options->backProjectionWidth);
      m_backprojectionRenderQuad.clearTargets(1.0f, util::vec4f(0.0f));
      m_backprojectionRenderQuad.setDepthLayer(lightIndex);
      m_backprojectionRenderQuad.setWriteFrameBuffer();
    }

    void IndirectShadowsCreation::unsetBackprojectionMap()
    {
      m_backprojectionRenderQuad.unsetWriteFrameBuffer();
      glViewport(0, 0, m_options->width, m_options->height);
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

    util::SharedPointer<db::Texture3D> IndirectShadowsCreation::getBackprojectionDepthMaps() const
    {
      return m_geometryBackprojectionDepthMap;
    }

    util::SharedPointer<db::Texture3D> IndirectShadowsCreation::getBackprojectionPositionMaps() const
    {
      return m_geometryBackprojectionPositionMap;
    }

    util::SharedPointer<db::Texture3D> IndirectShadowsCreation::getBackprojectionNormalMaps() const
    {
      return m_geometryBackprojectionNormalMap;
    }
  }
}
