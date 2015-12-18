#include "Renderer/Pipeline/DelaunayTriangulation.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/RandomSequenceGenerator.h>
#include <Utilities/Timer/Timer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    struct DrawArraysIndirectCommand
    {
      GLuint count;//number of indices
      GLuint instanceCount;//the number of instances
      GLuint baseVertex;//offset which adds to every index value
      GLuint baseInstance;//base instance, getting added to all vertex attribute divisors, not to gl_InstanceID!
    };

    DelaunayTriangulation::DelaunayTriangulation() : m_occluderNumber(0)
    {
      glGenVertexArrays(1, &m_siteVAO);

      m_ping = 1;
      m_pong = 1 - m_ping;
    }

    DelaunayTriangulation::~DelaunayTriangulation()
    {
      glDeleteVertexArrays(1, &m_siteVAO);
    }

    void DelaunayTriangulation::initialize(util::SingletonManager *singletonManager)
    {
      glBindVertexArray(m_siteVAO);
      glVertexAttribFormat(sh::RenderShader::POSITION, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::POSITION, 0);
      glVertexAttribFormat(sh::RenderShader::NORMAL, 4, GL_FLOAT, GL_FALSE, sizeof(util::vec4f));
      glVertexAttribBinding(sh::RenderShader::NORMAL, 0);
      glBindVertexArray(0);

      m_options = singletonManager->getService<RenderOptions>();

      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_siteProjectionShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::PARABOLOIDCACHETOSITEPROJECTION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION));
      m_voronoiCreationShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::JUMPFLOODALGORITHM, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_sampleOffsetShaderHandle = sh::ShaderContainer::INDIRECTSHADOWPOINTSAMPLINGOFFSETCALCULATION;
      m_sampleOffsetSummationShaderHandle = sh::ShaderContainer::INDIRECTSHADOWPOINTSAMPLINGOFFSETSUMMATION;
      m_delaunayTriangulationShaderHandle = sh::ShaderContainer::INDIRECTSHADOWPOINTSAMPLING;
      m_showVoronoiDiagramShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SHOWVORONOIDIAGRAM, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));

      DrawArraysIndirectCommand command;
      command.baseInstance = 0;
      command.baseVertex = 0;
      command.instanceCount = m_options->giLightSampleNumber / (m_options->indirectShadowMapTilenumber * m_options->giShadowLightSampleDivisor);
      m_sampleCommandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand), sizeof(DrawArraysIndirectCommand), GL_STATIC_DRAW, &command);

      m_sampleNumberOffset = util::SharedPointer<db::Texture1D>(new db::Texture1D(m_options->backProjectionWidth, GL_TEXTURE_1D, GL_UNSIGNED_INT, GL_R16UI, GL_RED_INTEGER, 1, 16));

      m_voronoiDiagram[0] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->backProjectionWidth, m_options->backProjectionWidth, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R32UI, GL_RED_INTEGER, 1, 32));
      m_voronoiDiagram[1] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->backProjectionWidth, m_options->backProjectionWidth, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R32UI, GL_RED_INTEGER, 1, 32));
      //m_voronoiDiagram[0] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->backProjectionWidth, m_options->backProjectionWidth, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      //m_voronoiDiagram[1] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->backProjectionWidth, m_options->backProjectionWidth, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));

      m_adaptiveCacheSamplesPosition.createBuffer(GL_TEXTURE_BUFFER, m_options->adaptiveSampleNumber * sizeof(util::vec4f), m_options->adaptiveSampleNumber * sizeof(util::vec4f), GL_STATIC_DRAW, GL_RGBA32F);
      m_adaptiveCacheSamplesNormals.createBuffer(GL_TEXTURE_BUFFER, m_options->adaptiveSampleNumber * sizeof(util::vec4f), m_options->adaptiveSampleNumber * sizeof(util::vec4f), GL_STATIC_DRAW, GL_RGBA32F);

      std::vector<util::vec2f> noise(m_options->adaptiveSampleNumber);
      for(unsigned int i = 0; i < m_options->adaptiveSampleNumber; i++)
      {
        noise[i] = util::vec2f(util::RandomSequenceGenerator::halton(i + 1, 2), util::RandomSequenceGenerator::halton(i + 1, 3));
      }

      m_noiseTexture = util::SharedPointer<db::Texture1D>(new db::Texture1D(m_options->adaptiveSampleNumber, GL_TEXTURE_1D, GL_FLOAT, GL_RG16F, GL_RG, 2, 32, &noise[0]));
    }

    void DelaunayTriangulation::updateBuffer(unsigned int occluderNumber)
    {
      if(occluderNumber != m_occluderNumber)
      {
        m_occluderNumber = occluderNumber;

        m_projectedSites.createBuffer(GL_TEXTURE_BUFFER, m_occluderNumber * sizeof(util::vec2f), m_occluderNumber * sizeof(util::vec2f), GL_STATIC_DRAW, GL_RG32F);

        std::vector<util::vec3ub> siteColors(m_occluderNumber);
        for(unsigned int i = 0; i < m_occluderNumber; i++)
        {
          siteColors[i] = util::vec3ub(255 * util::RandomSequenceGenerator::halton(i + 1, 2), 255 * util::RandomSequenceGenerator::halton(i + 1, 3), 255 * util::RandomSequenceGenerator::halton(i + 1, 5));
        }

        m_voronoiColorTexture = util::SharedPointer<db::Texture1D>(new db::Texture1D(m_occluderNumber, GL_TEXTURE_1D, GL_UNSIGNED_BYTE, GL_RGB8, GL_RGB, 3, 24, &siteColors[0]));
      }
    }

    void DelaunayTriangulation::calculateDelaunayTriangulation(
      util::SharedPointer<db::Texture3D> backprojectionZBuffer,
      util::SharedPointer<db::Texture3D> backprojectionPosition, 
      util::SharedPointer<db::Texture3D> backprojectionNormal,
      const GPUBuffer& occluderNormalCoordinates,
      const GPUImmutableBuffer& reflectiveShadowLightsBuffer)
    {
      glViewport(0, 0, m_options->backProjectionWidth, m_options->backProjectionWidth);

      generateProjectedSiteBuffer(backprojectionZBuffer, occluderNormalCoordinates, reflectiveShadowLightsBuffer);
      generateVoronoiDiagram();
      generateTriangles(backprojectionPosition, backprojectionNormal);

      glViewport(0, 0, m_options->width, m_options->height);
    }

    void DelaunayTriangulation::showVoronoiDiagram()
    {
      glViewport(0, 0, m_options->backProjectionWidth, m_options->backProjectionWidth);

      const sh::RenderShader& showVoronoiShader = m_shaderContainer->getRenderShader(m_showVoronoiDiagramShaderHandle);

      showVoronoiShader.useShader();

      sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &m_occluderNumber);

      m_voronoiDiagram[m_ping]->setTexture(0, 0);
      m_voronoiColorTexture->setTexture(1, 1);

      m_voronoiRenderQuad.render();

      showVoronoiShader.useNoShader();

      glViewport(0, 0, m_options->width, m_options->height);
    }

    util::SharedPointer<db::Texture2D> DelaunayTriangulation::getVoronoiDiagram()
    {
      return m_voronoiDiagram[m_ping];
    }

    const TBO& DelaunayTriangulation::getAdaptiveSamplePositionBuffer()
    {
      return m_adaptiveCacheSamplesPosition;
    }

    const TBO& DelaunayTriangulation::getAdaptiveSampleNormalBuffer()
    {
      return m_adaptiveCacheSamplesNormals;
    }

    const GPUBuffer& DelaunayTriangulation::getCommandBuffer()
    {
      return m_sampleCommandBuffer;
    }

    void DelaunayTriangulation::generateProjectedSiteBuffer(util::SharedPointer<db::Texture3D> backprojectionZBuffer, const GPUBuffer& occluderNormalCoordinates, const GPUImmutableBuffer& reflectiveShadowLightsBuffer)
    {
      //CPUTIMER("cpuCompute", 0)
      //GPUTIMER("gpuCompute", 1)

      unsigned int clearVal = 0;
      m_voronoiDiagram[m_pong]->clearTexture(&clearVal);
      m_voronoiDiagram[m_ping]->clearTexture(&clearVal);

      glDepthMask(GL_FALSE);
      glDepthFunc(GL_LEQUAL);

      m_siteProjectionRenderQuad.setRenderTargets(backprojectionZBuffer, 0, 1, m_voronoiDiagram[m_pong]);

      m_siteProjectionRenderQuad.setWriteFrameBuffer();

      glClear(GL_COLOR_BUFFER_BIT);

      const sh::RenderShader& delaunayShader = m_shaderContainer->getRenderShader(m_siteProjectionShaderHandle);

      delaunayShader.useShader();

      glBindVertexArray(m_siteVAO);
      glEnableVertexAttribArray(sh::RenderShader::POSITION);

      occluderNormalCoordinates.bindVertexbuffer(0, 0, 2 * sizeof(util::vec4f));

      reflectiveShadowLightsBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      
      m_projectedSites.bindImageTexture(0, 0, GL_WRITE_ONLY, GL_RG32F);

      sh::RenderShader::setUniform(0, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);

      unsigned int lightIndex = 0;
      sh::RenderShader::setUniform(1, GL_UNSIGNED_INT, &lightIndex);

      glDrawArrays(GL_POINTS, 0, m_occluderNumber);

      m_projectedSites.unbindImageTexture(sh::RenderShader::POSITION, 0, GL_WRITE_ONLY, GL_RG32F);

      reflectiveShadowLightsBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      occluderNormalCoordinates.unbindVertexBuffer(0);

      glDisableVertexAttribArray(sh::RenderShader::POSITION);
      glBindVertexArray(0);

      delaunayShader.useNoShader();

      m_siteProjectionRenderQuad.unsetWriteFrameBuffer();

      glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);

      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

      //std::vector<util::vec2f> siteData(m_cacheNumber);
      //m_projectedSites.getData(0, m_cacheNumber * sizeof(util::vec2f), &siteData[0]);

      //std::vector<float> zData(m_options->backProjectionWidth * m_options->backProjectionWidth);
      //m_zBuffer->getTextureData(&zData[0]);
    }

    void DelaunayTriangulation::generateVoronoiDiagram()
    {
      unsigned int stepNumber = log(float(m_options->backProjectionWidth)) / log(2.0f);

      const sh::RenderShader& voronoiShader = m_shaderContainer->getRenderShader(m_voronoiCreationShaderHandle);

      voronoiShader.useShader();

      m_projectedSites.bindTexture(0);

      sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &m_options->backProjectionWidth);

      for(int i = stepNumber - 1; i >= 0; i--)
      {
        m_voronoiRenderQuad.setRenderTargets(1, m_voronoiDiagram[m_ping]);

        m_voronoiRenderQuad.setWriteFrameBuffer();

        m_voronoiDiagram[m_pong]->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32UI);

        int stepSize = powf(2.0f, i);
        sh::RenderShader::setUniform(1, GL_INT, &stepSize);

        m_voronoiRenderQuad.render();

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        if(i > 0)
        {
          m_ping = m_pong;
          m_pong = 1 - m_ping;
        }
      }

      m_voronoiDiagram[m_pong]->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32UI);

      m_projectedSites.unbindTexture(0);

      m_voronoiRenderQuad.unsetWriteFrameBuffer();

      voronoiShader.useNoShader();

      //std::vector<unsigned int> indexData0(m_options->backProjectionWidth * m_options->backProjectionWidth);
      //m_voronoiDiagram[m_pong]->getTextureData(&indexData0[0]);

      //std::vector<unsigned int> indexData1(m_options->backProjectionWidth * m_options->backProjectionWidth);
      //m_voronoiDiagram[m_ping]->getTextureData(&indexData1[0]);
    }

    void DelaunayTriangulation::generateTriangles(util::SharedPointer<db::Texture3D> backprojectionPosition, util::SharedPointer<db::Texture3D> backprojectionNormal)
    {
      {
        sh::ComputeShader sampleNumerOffsetCalculation = m_shaderContainer->getComputeShader(m_sampleOffsetShaderHandle);

        sampleNumerOffsetCalculation.useShader();

        m_voronoiDiagram[m_ping]->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32UI);
        m_sampleNumberOffset->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_R16UI);

        m_projectedSites.bindTexture(0);

        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT, &m_options->backProjectionWidth);

        sh::ComputeShader::setUniform(2, GL_UNSIGNED_INT, &m_options->adaptiveSampleNumber);

        sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        m_projectedSites.unbindTexture(0);

        m_sampleNumberOffset->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_R16UI);
        m_voronoiDiagram[m_ping]->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32UI);

        sampleNumerOffsetCalculation.useNoShader();
      }

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);

      //std::vector<unsigned int> zData(m_options->backProjectionWidth);
      //m_sampleNumberOffset->getTextureData(&zData[0]);

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      {
        sh::ComputeShader sampleNumberOffsetSummation = m_shaderContainer->getComputeShader(m_sampleOffsetSummationShaderHandle);

        sampleNumberOffsetSummation.useShader();

        m_sampleNumberOffset->bindImageTexture(0, 0, GL_READ_WRITE, GL_R16UI);

        m_sampleCommandBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

        sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &m_options->backProjectionWidth);

        sh::ComputeShader::dispatchComputeShader(1, 1, 1);

        m_sampleCommandBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

        m_sampleNumberOffset->unbindImageTexture(0, 0, GL_READ_WRITE, GL_R16UI);

        sampleNumberOffsetSummation.useNoShader();
      }

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);

      //std::vector<unsigned int> zData2(m_options->backProjectionWidth);
      //m_sampleNumberOffset->getTextureData(&zData2[0]);

      //DrawArraysIndirectCommand command;
      //m_sampleCommandBuffer.getData(0, sizeof(DrawArraysIndirectCommand), &command);

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      {
        sh::ComputeShader delaunayTriangulation = m_shaderContainer->getComputeShader(m_delaunayTriangulationShaderHandle);

        delaunayTriangulation.useShader();

        m_voronoiDiagram[m_ping]->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32UI);
        m_sampleNumberOffset->bindImageTexture(1, 0, GL_READ_ONLY, GL_R16UI);

        backprojectionPosition->bindImageTexture(2, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        backprojectionNormal->bindImageTexture(3, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        m_adaptiveCacheSamplesPosition.bindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA32F);
        m_adaptiveCacheSamplesNormals.bindImageTexture(5, 0, GL_WRITE_ONLY, GL_RGBA32F);

        m_noiseTexture->bindImageTexture(6, 0, GL_READ_ONLY, GL_RG16F);

        m_projectedSites.bindTexture(0);

        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT, &m_options->backProjectionWidth);

        sh::ComputeShader::setUniform(2, GL_UNSIGNED_INT, &m_options->adaptiveSampleNumber);

        sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        m_projectedSites.unbindTexture(0);

        m_noiseTexture->unbindImageTexture(6, 0, GL_READ_ONLY, GL_RG16F);

        m_adaptiveCacheSamplesNormals.unbindImageTexture(5, 0, GL_WRITE_ONLY, GL_RGBA32F);
        m_adaptiveCacheSamplesPosition.unbindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA32F);

        backprojectionNormal->unbindImageTexture(3, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        backprojectionPosition->unbindImageTexture(2, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        m_sampleNumberOffset->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R16UI);
        m_voronoiDiagram[m_ping]->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32UI);

        delaunayTriangulation.useNoShader();

        //glMemoryBarrier(GL_ALL_BARRIER_BITS);

        //std::vector<util::vec4f> origPosData(m_options->backProjectionWidth * m_options->backProjectionWidth);
        //m_backprojectionPosition->getTextureData(&origPosData[0]);

        //std::vector<util::vec4f> posData(m_options->adaptiveSampleNumber);
        //m_adaptiveCacheSamplesPosition.getData(0, m_options->adaptiveSampleNumber * sizeof(util::vec4f), &posData[0]);

        //std::vector<util::vec4f> normalData(m_options->adaptiveSampleNumber);
        //m_adaptiveCacheSamplesNormals.getData(0, m_options->adaptiveSampleNumber * sizeof(util::vec4f), &normalData[0]);
      }
    }
  }
}
