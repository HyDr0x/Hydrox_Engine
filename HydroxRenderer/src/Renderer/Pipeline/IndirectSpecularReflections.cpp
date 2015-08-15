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
    struct DrawArraysIndirectCommand
    {
      GLuint count;//number of indices
      GLuint primCount;//the number of primitives
      GLuint first;//offset which adds to every index value
      GLuint baseInstance;//base instance, getting added to all vertex attribute divisors, not to gl_InstanceID!
    };

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

      m_calculateAreaLightTube = sh::ShaderContainer::SPECULARTUBECREATION;
      m_specularCacheXCreation = sh::ShaderContainer::SPECULARCACHEPOSITIONCREATIONX;
      m_specularCacheYCreation = sh::ShaderContainer::SPECULARCACHEPOSITIONCREATIONY; 
      m_specularCacheOffsetUpSweep = sh::ShaderContainer::SPECULARCACHEOFFSETUPSWEEP;
      m_specularCacheOffsetDownSweep = sh::ShaderContainer::SPECULARCACHEOFFSETDOWNSWEEP;
      m_specularCacheCreation = sh::ShaderContainer::SPECULARCACHECREATION;
      m_specularProxyLightCreation = sh::ShaderContainer::SPECULARPROXYLIGHTCREATION;
      m_specularCacheIndexMapCreation = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SPECULARCACHEINDEXMAPCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION) | sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::NORMAL));
      m_specularLightMapCreation = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SPECULARLIGHTMAPCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));

      m_specularLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));

      m_cachePositionBuffer0 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_cachePositionBuffer1 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_cacheOffsets = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));

      m_cacheIndexMap.resize(4);
      m_cacheIndexMap[0] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R16UI, GL_RED_INTEGER, 1, 16));
      m_cacheIndexMap[1] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R16UI, GL_RED_INTEGER, 1, 16));
      m_cacheIndexMap[2] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R16UI, GL_RED_INTEGER, 1, 16));
      m_cacheIndexMap[3] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R16UI, GL_RED_INTEGER, 1, 16));

      m_cacheIndexAtomicCounter = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R32UI, GL_RED_INTEGER, 1, 32));

      m_specularLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));

      m_specularProxyLightQuad.setRenderTargets(1, m_specularLightMap);

      m_proxyLightPositionBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_proxyLightLuminousFluxBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_cachePositions.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_cacheNormalMaterial.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);

      DrawArraysIndirectCommand drawCommand;
      drawCommand.baseInstance = 0;
      drawCommand.first = 0;
      drawCommand.primCount = 1;
      m_commandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand), sizeof(DrawArraysIndirectCommand), GL_STATIC_DRAW, &drawCommand);
    }

    void IndirectSpecularReflections::updateBuffer(unsigned int reflectiveShadowMapNumber)
    {
      if(reflectiveShadowMapNumber != m_reflectiveShadowMapNumber)
      {
        m_reflectiveShadowMapNumber = reflectiveShadowMapNumber;

        m_proxyLightTubeBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_reflectiveShadowMapNumber * 2 * sizeof(util::vec4f), 2 * sizeof(util::vec4f), GL_STREAM_COPY);
      }
    }

    void IndirectSpecularReflections::generateReflectionCaches(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap,
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      util::SharedPointer<db::Texture3D> indirectLightNormals,
      util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
      const GPUBuffer& samplingPatternBuffer)
    {
      createTubeData(indirectLightPositions, samplingPatternBuffer);

      createSpecularCachePositions(gBufferDepthMap, gBufferNormalMap, gBufferMaterialMap);

      createProxyLightOffsets();

      createSpecularCaches(gBufferDepthMap, gBufferNormalMap, gBufferMaterialMap);

      createProxyLights(indirectLightPositions, indirectLightNormals, indirectLightLuminousFlux, samplingPatternBuffer);

      createSpecularCacheIndexMap(gBufferDepthMap, gBufferNormalMap);

      createSpecularProxyLightMap(gBufferDepthMap, gBufferNormalMap, gBufferMaterialMap);
    }

    void IndirectSpecularReflections::createTubeData(util::SharedPointer<db::Texture3D> indirectLightPositions, const GPUBuffer& samplingPatternBuffer)
    {
      const sh::ComputeShader& shader = m_shaderContainer->getComputeShader(m_calculateAreaLightTube);
      shader.useShader();

      indirectLightPositions->setTexture(0, 0);

      samplingPatternBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_proxyLightTubeBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

      sh::ComputeShader::dispatchComputeShader(indirectLightPositions->getResolution()[2], 1, 1);

      m_proxyLightTubeBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      samplingPatternBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      indirectLightPositions->unsetTexture(0);

      shader.useNoShader();
    }

    void IndirectSpecularReflections::createSpecularCachePositions(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap)
    {

      float clearValue = 0.0f;
      m_cachePositionBuffer0->clearTexture(&clearValue);
      m_cachePositionBuffer1->clearTexture(&clearValue);
      m_cacheOffsets->clearTexture(&clearValue);

      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      const sh::ComputeShader& specularXCreationShader = m_shaderContainer->getComputeShader(m_specularCacheXCreation);
      specularXCreationShader.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);

      sh::ComputeShader::setUniform(3, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);

      m_cachePositionBuffer0->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);

      m_proxyLightTubeBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_proxyLightTubeBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      m_cachePositionBuffer0->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);

      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      specularXCreationShader.useNoShader();


      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


      const sh::ComputeShader& specularYCreationShader = m_shaderContainer->getComputeShader(m_specularCacheYCreation);
      specularYCreationShader.useShader();

      gBufferDepthMap->setTexture(0, 0);

      m_cachePositionBuffer0->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);
      m_cachePositionBuffer1->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_R32F);
      m_cacheOffsets->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_R32F);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_cacheOffsets->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_R32F);
      m_cachePositionBuffer1->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_R32F);
      m_cachePositionBuffer0->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      gBufferDepthMap->unsetTexture(0);

      specularYCreationShader.useNoShader();
    }

    void IndirectSpecularReflections::createProxyLightOffsets()
    {
      const sh::ComputeShader& specularCacheOffsetUpSweepShader = m_shaderContainer->getComputeShader(m_specularCacheOffsetUpSweep);
      specularCacheOffsetUpSweepShader.useShader();

      unsigned int pixelNumber = m_options->width * m_options->height;
      unsigned int workGroupSize = 128;
      unsigned int workGroupNumber;
      unsigned int pixelStride = 1;

      do
      {
        workGroupNumber = std::min<unsigned int>(ceil(pixelNumber / float(pixelStride * 2 * workGroupSize)), 128);

        m_cacheOffsets->bindImageTexture(0, 0, GL_READ_WRITE, GL_R32F);

        sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &pixelStride);

        sh::ComputeShader::dispatchComputeShader(workGroupNumber, 1, 1);

        m_cacheOffsets->unbindImageTexture(0, 0, GL_READ_WRITE, GL_R32F);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        pixelStride *= 2 * workGroupSize;

      } while(workGroupNumber > 1);

      specularCacheOffsetUpSweepShader.useNoShader();


      const sh::ComputeShader& specularCacheOffsetDownSweepShader = m_shaderContainer->getComputeShader(m_specularCacheOffsetDownSweep);
      specularCacheOffsetDownSweepShader.useShader();

      unsigned int roundedPixelNumber = powf(2.0f, ceil(logf(pixelNumber) / logf(2.0f)));
      unsigned int actualSampleRangePerWorkGroup = roundedPixelNumber;
      
      do
      {
        pixelStride = ceil(actualSampleRangePerWorkGroup / float(workGroupSize));
        workGroupNumber = std::min<unsigned int>(ceil(pixelNumber / float(actualSampleRangePerWorkGroup)), 128);
        unsigned int pixelPerThread = ceil(roundedPixelNumber / float(pixelStride * workGroupNumber * workGroupSize));

        m_cacheOffsets->bindImageTexture(0, 0, GL_READ_WRITE, GL_R32F);

        sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &pixelStride);
        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT, &actualSampleRangePerWorkGroup);
        sh::ComputeShader::setUniform(2, GL_UNSIGNED_INT, &pixelPerThread);

        sh::ComputeShader::dispatchComputeShader(workGroupNumber, 1, 1);

        m_cacheOffsets->unbindImageTexture(0, 0, GL_READ_WRITE, GL_R32F);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        actualSampleRangePerWorkGroup = pixelStride;
      } while(pixelStride > 1);

      specularCacheOffsetDownSweepShader.useNoShader();

      //std::vector<float> offsets(m_options->width * m_options->height);
      //m_cacheOffsets->getTextureData(&offsets[0]);

      //unsigned int endVal = offsets[m_options->width * m_options->height - 1];
      //unsigned int endVal1 = offsets[65535];
      //unsigned int endVal2 = offsets[131071];
      //unsigned int endVal3 = offsets[196607];
      //unsigned int endVal4 = offsets[8 * 65536 - 1];
    }

    void IndirectSpecularReflections::createSpecularCaches(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap)
    {
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      const sh::ComputeShader& specularCacheCreationShader = m_shaderContainer->getComputeShader(m_specularCacheCreation);
      specularCacheCreationShader.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);

      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_commandBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

      m_cacheOffsets->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);
      m_cachePositionBuffer1->bindImageTexture(1, 0, GL_READ_ONLY, GL_R32F);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_cachePositionBuffer1->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R32F);
      m_cacheOffsets->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      m_commandBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      specularCacheCreationShader.useNoShader();


      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<util::vec4f> cachePos(m_options->specularCacheNumber);
      //m_cachePositions.getData(0, m_options->specularCacheNumber * sizeof(util::vec4f), &cachePos[0]);

      //std::vector<util::vec4f> cacheNormalMat(m_options->specularCacheNumber);
      //m_cacheNormalMaterial.getData(0, m_options->specularCacheNumber * sizeof(util::vec4f), &cacheNormalMat[0]);
    }

    void IndirectSpecularReflections::createProxyLights(
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      util::SharedPointer<db::Texture3D> indirectLightNormals,
      util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
      const GPUBuffer& samplingPatternBuffer)
    {
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      const sh::ComputeShader& specularProxyLightCreationShader = m_shaderContainer->getComputeShader(m_specularProxyLightCreation);
      specularProxyLightCreationShader.useShader();

      indirectLightPositions->setTexture(0, 0);
      indirectLightNormals->setTexture(1, 1);
      indirectLightLuminousFlux->setTexture(2, 2);

      sh::ComputeShader::setUniform(3, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);

      samplingPatternBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_proxyLightPositionBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_proxyLightLuminousFluxBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      
      m_cacheOffsets->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_cacheOffsets->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R32F);

      m_proxyLightLuminousFluxBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_proxyLightPositionBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      samplingPatternBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      indirectLightLuminousFlux->unsetTexture(2);
      indirectLightNormals->unsetTexture(1);
      indirectLightPositions->unsetTexture(0);

      specularProxyLightCreationShader.useNoShader();


      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<util::vec4f> proxyLightPositionData(m_options->specularCacheNumber);
      //m_proxyLightPositionBuffer.getData(0, m_options->specularCacheNumber * sizeof(util::vec4f), &proxyLightPositionData[0]);

      //std::vector<util::vec4f> proxyLightLuminousFluxData(m_options->specularCacheNumber);
      //m_proxyLightLuminousFluxBuffer.getData(0, m_options->specularCacheNumber * sizeof(util::vec4f), &proxyLightLuminousFluxData[0]);
    }

    void IndirectSpecularReflections::createSpecularCacheIndexMap(util::SharedPointer<db::Texture2D> gBufferDepthMap, util::SharedPointer<db::Texture2D> gBufferNormalMap)
    {
      unsigned int clearValue = 0;
      m_cacheIndexMap[0]->clearTexture(&clearValue);
      m_cacheIndexMap[1]->clearTexture(&clearValue);
      m_cacheIndexMap[2]->clearTexture(&clearValue);
      m_cacheIndexMap[3]->clearTexture(&clearValue);

      m_cacheIndexAtomicCounter->clearTexture(&clearValue);


      util::vec4f clearValue2 = util::vec4f(0.0f);
      m_specularLightMap->clearTexture(&clearValue2[0]);


      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

      m_specularIndexQuad.setRenderTargets(gBufferDepthMap, 1, m_specularLightMap);
      m_specularIndexQuad.setWriteFrameBuffer();

      glDepthMask(GL_FALSE);
      glDepthFunc(GL_LEQUAL);

      glBindVertexArray(m_pointVAO);
      glEnableVertexAttribArray(sh::RenderShader::POSITION);
      glEnableVertexAttribArray(sh::RenderShader::NORMAL);

      const sh::RenderShader& specularCacheIndexMapCreationShader = m_shaderContainer->getRenderShader(m_specularCacheIndexMapCreation);
      specularCacheIndexMapCreationShader.useShader();

      m_cacheIndexMap[0]->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R16UI);
      m_cacheIndexMap[1]->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_R16UI);
      m_cacheIndexMap[2]->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16UI);
      m_cacheIndexMap[3]->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_R16UI);

      m_cacheIndexAtomicCounter->bindImageTexture(4, 0, GL_READ_WRITE, GL_R32UI);

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);

      m_cachePositions.bindVertexbuffer(0, 0, sizeof(util::vec4f));
      m_cacheNormalMaterial.bindVertexbuffer(1, 0, sizeof(util::vec4f));
      m_commandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);

      glDrawArraysIndirect(GL_POINTS, nullptr);

      m_commandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);
      m_cacheNormalMaterial.unbindVertexBuffer(1);
      m_cachePositions.unbindVertexBuffer(0);

      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      m_cacheIndexAtomicCounter->unbindImageTexture(4, 0, GL_READ_WRITE, GL_R32UI);

      m_cacheIndexMap[3]->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_R16UI);
      m_cacheIndexMap[2]->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_R16UI);
      m_cacheIndexMap[1]->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_R16UI);
      m_cacheIndexMap[0]->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R16UI);

      specularCacheIndexMapCreationShader.useNoShader();

      glDisableVertexAttribArray(sh::RenderShader::NORMAL);
      glDisableVertexAttribArray(sh::RenderShader::POSITION);
      glBindVertexArray(0);

      glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);

      m_specularIndexQuad.unsetWriteFrameBuffer();

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<unsigned int> cacheIndices0(m_options->width * m_options->height);
      //m_cacheIndexMap[0]->getTextureData(&cacheIndices0[0]);

      //std::vector<unsigned int> cacheIndices1(m_options->width * m_options->height);
      //m_cacheIndexMap[1]->getTextureData(&cacheIndices1[0]);

      //std::vector<unsigned int> cacheIndices2(m_options->width * m_options->height);
      //m_cacheIndexMap[2]->getTextureData(&cacheIndices2[0]);

      //std::vector<unsigned int> cacheIndices3(m_options->width * m_options->height);
      //m_cacheIndexMap[3]->getTextureData(&cacheIndices3[0]);
    }

    void IndirectSpecularReflections::createSpecularProxyLightMap(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap)
    {
      util::vec4f clearValue = util::vec4f(0.0f);
      m_specularLightMap->clearTexture(&clearValue[0]);

      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      m_specularProxyLightQuad.setWriteFrameBuffer();

      const sh::RenderShader& specularLightMapCreationShader = m_shaderContainer->getRenderShader(m_specularLightMapCreation);
      specularLightMapCreationShader.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);

      m_cacheIndexMap[0]->bindImageTexture(0, 0, GL_READ_ONLY, GL_R16UI);
      m_cacheIndexMap[1]->bindImageTexture(1, 0, GL_READ_ONLY, GL_R16UI);
      m_cacheIndexMap[2]->bindImageTexture(2, 0, GL_READ_ONLY, GL_R16UI);
      m_cacheIndexMap[3]->bindImageTexture(3, 0, GL_READ_ONLY, GL_R16UI);

      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_proxyLightPositionBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_proxyLightLuminousFluxBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

      m_specularProxyLightQuad.render();

      m_proxyLightLuminousFluxBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_proxyLightPositionBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_cacheIndexMap[3]->unbindImageTexture(3, 0, GL_READ_ONLY, GL_R16UI);
      m_cacheIndexMap[2]->unbindImageTexture(2, 0, GL_READ_ONLY, GL_R16UI);
      m_cacheIndexMap[1]->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R16UI);
      m_cacheIndexMap[0]->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R16UI);

      gBufferMaterialMap->setTexture(2, 2);
      gBufferNormalMap->setTexture(1, 1);
      gBufferDepthMap->setTexture(0, 0);

      specularLightMapCreationShader.useNoShader();

      m_specularProxyLightQuad.unsetWriteFrameBuffer();
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugCachePositionsMap() const
    {
      return m_cachePositionBuffer1;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugIndexCounterMap() const
    {
      return m_cacheIndexAtomicCounter;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getSpecularLightMap() const
    {
      return m_specularLightMap; 
    }
  }
}
