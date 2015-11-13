#include "Renderer/Pipeline/IndirectSpecularReflections.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/RandomSequenceGenerator.h>
#include <Utilities/Timer/Timer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"
#include "Renderer/Pipeline/CameraManager.h"

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

    struct DrawElementsIndirectCommand
    {
      GLuint count;//number of indices
      GLuint primCount;//the number of primitives
      GLuint first;//offset which adds to every index value
      GLuint baseVertex;
      GLuint baseInstance;//base instance, getting added to all vertex attribute divisors, not to gl_InstanceID!
    };

    IndirectSpecularReflections::IndirectSpecularReflections() : m_ping(0), m_pong(1 - m_ping)
    {
    }

    IndirectSpecularReflections::~IndirectSpecularReflections()
    {
    }

    void IndirectSpecularReflections::initialize(util::SingletonManager *singletonManager)
    {
      m_options = singletonManager->getService<RenderOptions>();

      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_gBufferDownsampling = sh::ShaderContainer::GBUFFERDOWNSAMPLING;
      m_calculateAreaLightTube = sh::ShaderContainer::SPECULARTUBECREATION;
      m_specularCachePositionFilterX = sh::ShaderContainer::SPECULARCACHEPOSITIONFILTERX;
      m_specularCachePositionFilterY = sh::ShaderContainer::SPECULARCACHEPOSITIONFILTERY;
      m_specularCacheOffsetUpSweep = sh::ShaderContainer::SPECULARCACHEOFFSETUPSWEEP;
      m_specularCacheOffsetDownSweep = sh::ShaderContainer::SPECULARCACHEOFFSETDOWNSWEEP;
      m_specularCacheBufferCreation = sh::ShaderContainer::SPECULARCACHECREATION;
      m_specularProxyLightBufferCreation = sh::ShaderContainer::SPECULARPROXYLIGHTCREATION;
      m_specularCacheIndexListGeneration = sh::ShaderContainer::SPECULARCACHEINDEXLISTCREATION;
      m_specularCacheEdgeCacheCreation = sh::ShaderContainer::SPECULARCACHEEDGECACHECREATION;
      m_specularCacheEdgeVertexCreation = sh::ShaderContainer::SPECULARCACHEEDGEVERTEXCREATION;
      m_indirectLightMapCreation = sh::ShaderContainer::INDIRECTLIGHTMAPCREATION;
      m_indirectLightMapUpsampling = sh::ShaderContainer::INDIRECTLIGHTUPSAMPLING;

      m_vertexResolution = m_options->vertexTileSize;
      m_vertexIndexMultiplicator = m_options->vertexTileMulti;
      m_cacheResolution = m_options->cacheTileSize;

      m_indirectLightResolution = util::vec2i(m_options->width / 2, m_options->height / 2);
      m_halfResGBufferDepthMap0 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_halfResGBufferLinearDepthMap0 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_halfResGBufferNormalMap0 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_halfResGBufferVertexNormalMap0 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_halfResGBufferMaterialMap0 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));

      m_halfResGBufferDepthMap1 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_halfResGBufferLinearDepthMap1 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_halfResGBufferNormalMap1 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_halfResGBufferVertexNormalMap1 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_halfResGBufferMaterialMap1 = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));

      m_cachePositionBufferInnerX = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));

      m_cacheAtomicIndexMap = util::SharedPointer<db::Texture2D>(new db::Texture2D((m_indirectLightResolution[0] + m_cacheResolution - 1) / m_cacheResolution, (m_indirectLightResolution[1] + m_cacheResolution - 1) / m_cacheResolution, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R32UI, GL_RED_INTEGER, 1, 32));
      m_cacheOffsetMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_cachePositionMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_cacheNormalMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));

      m_vertexAtomicIndexMap = util::SharedPointer<db::Texture2D>(new db::Texture2D((m_indirectLightResolution[0] + m_vertexResolution * m_vertexIndexMultiplicator - 1) / (m_vertexResolution * m_vertexIndexMultiplicator), (m_indirectLightResolution[1] + m_vertexResolution * m_vertexIndexMultiplicator - 1) / (m_vertexResolution * m_vertexIndexMultiplicator), GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R32UI, GL_RED_INTEGER, 1, 32));
      m_vertexPositionMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_vertexNormalMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));

      //m_specularLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_specularLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_indirectLightResolution[0], m_indirectLightResolution[1], GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_upsampledSpecularLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));

      m_proxyLightPositionBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), 2 * m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_proxyLightLuminousFluxBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), 2 * m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);

      m_cachePositions.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_cacheNormalMaterial.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);

      m_cacheIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 8 * m_indirectLightResolution[0] * m_indirectLightResolution[1] * sizeof(unsigned int), 8 * m_indirectLightResolution[0] * m_indirectLightResolution[1] * sizeof(unsigned int), GL_STATIC_DRAW);

      m_debugCacheVertexIndexPositions.createBuffer(GL_ARRAY_BUFFER, 16 * m_options->specularCacheNumber * sizeof(util::vec4f), 16 * m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
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
      util::SharedPointer<db::Texture2D> gBufferLinearDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferVertexNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap,
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      util::SharedPointer<db::Texture3D> indirectLightNormals,
      util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
      util::SharedPointer<db::Texture3D> indirectShadowMaps,
      util::SharedPointer<db::Texture2D> indirectShadowVALQuaternions)
    {
      //CPUTIMER("MainloopCPUTimer", 0)
      //GPUTIMER("MainloopOGLTimer", 1)
      
      gBufferDownsampling(gBufferDepthMap, gBufferLinearDepthMap, gBufferNormalMap, gBufferVertexNormalMap, gBufferMaterialMap);

      createTubeData(indirectLightPositions);

      createSpecularEdgeCaches(m_halfResGBufferDepthMap1, m_halfResGBufferVertexNormalMap1, m_halfResGBufferMaterialMap1, m_halfResGBufferLinearDepthMap1);
      createEdgeVertices(m_halfResGBufferDepthMap1, m_halfResGBufferVertexNormalMap1, m_halfResGBufferMaterialMap1, m_halfResGBufferLinearDepthMap1);
      createSpecularInnerCaches(m_halfResGBufferDepthMap1, m_halfResGBufferVertexNormalMap1, m_halfResGBufferMaterialMap1);

      createOffsets(m_cacheOffsetMap);

      createSpecularCacheBuffer();

      createProxyLightBuffer(indirectLightPositions, indirectLightNormals, indirectLightLuminousFlux, indirectShadowMaps, indirectShadowVALQuaternions);

      createSpecularCacheIndices();

      createindirectLightMap(m_halfResGBufferDepthMap1, m_halfResGBufferNormalMap1, m_halfResGBufferMaterialMap1);

      indirectLightMapUpsampling();
    }

    void IndirectSpecularReflections::gBufferDownsampling(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                                                          util::SharedPointer<db::Texture2D> gBufferLinearDepthMap,
                                                          util::SharedPointer<db::Texture2D> gBufferNormalMap,
                                                          util::SharedPointer<db::Texture2D> gBufferVertexNormalMap,
                                                          util::SharedPointer<db::Texture2D> gBufferMaterialMap)
    {
      //CPUTIMER("GBufferDownsamplingCPUTimer", 0)
      //GPUTIMER("GBufferDownsamplingOGLTimer", 1)

      const sh::ComputeShader& shader = m_shaderContainer->getComputeShader(m_gBufferDownsampling);

      shader.useShader();

      util::vec2i resolution(m_options->width, m_options->height);
      sh::ComputeShader::setUniform(1, GL_INT_VEC2, &resolution[0]);
      sh::ComputeShader::setUniform(2, GL_INT_VEC2, &m_indirectLightResolution[0]);
      unsigned int minification = m_options->width / m_indirectLightResolution[0];
      sh::ComputeShader::setUniform(3, GL_UNSIGNED_INT, &minification);

      {
        gBufferDepthMap->setTexture(4, 4);
        gBufferLinearDepthMap->setTexture(5, 5);
        gBufferNormalMap->setTexture(6, 6);
        gBufferVertexNormalMap->setTexture(7, 7);
        gBufferMaterialMap->setTexture(8, 8);

        m_halfResGBufferDepthMap0->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);
        m_halfResGBufferLinearDepthMap0->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_R32F);
        m_halfResGBufferNormalMap0->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferVertexNormalMap0->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferMaterialMap0->bindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA16F);

        util::vec2i sampleDirection(1, 0);
        sh::ComputeShader::setUniform(0, GL_INT_VEC2, &sampleDirection[0]);

        sh::ComputeShader::dispatchComputeShader(1024, 1, 1);

        m_halfResGBufferMaterialMap0->unbindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferVertexNormalMap0->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferNormalMap0->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferLinearDepthMap0->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_R32F);
        m_halfResGBufferDepthMap0->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);

        gBufferMaterialMap->unsetTexture(8);
        gBufferVertexNormalMap->unsetTexture(7);
        gBufferNormalMap->unsetTexture(6);
        gBufferLinearDepthMap->unsetTexture(5);
        gBufferDepthMap->unsetTexture(4);
      }

      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

      {
        m_halfResGBufferDepthMap0->setTexture(4, 4);
        m_halfResGBufferLinearDepthMap0->setTexture(5, 5);
        m_halfResGBufferNormalMap0->setTexture(6, 6);
        m_halfResGBufferVertexNormalMap0->setTexture(7, 7);
        m_halfResGBufferMaterialMap0->setTexture(8, 8);

        m_halfResGBufferDepthMap1->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);
        m_halfResGBufferLinearDepthMap1->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_R32F);
        m_halfResGBufferNormalMap1->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferVertexNormalMap1->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferMaterialMap1->bindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA16F);

        util::vec2i sampleDirection(0, 1);
        sh::ComputeShader::setUniform(0, GL_INT_VEC2, &sampleDirection[0]);

        sh::ComputeShader::dispatchComputeShader(1024, 1, 1);

        m_halfResGBufferMaterialMap1->unbindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferVertexNormalMap1->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferNormalMap1->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA16F);
        m_halfResGBufferLinearDepthMap1->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_R32F);
        m_halfResGBufferDepthMap1->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);

        m_halfResGBufferMaterialMap0->unsetTexture(8);
        m_halfResGBufferVertexNormalMap0->unsetTexture(7);
        m_halfResGBufferNormalMap0->unsetTexture(6);
        m_halfResGBufferLinearDepthMap0->unsetTexture(5);
        m_halfResGBufferDepthMap0->unsetTexture(4);
      }

      shader.useNoShader();
    }

    void IndirectSpecularReflections::createTubeData(util::SharedPointer<db::Texture3D> indirectLightPositions)
    {
      //CPUTIMER("TubeCreationCPUTimer", 0)
      //GPUTIMER("TubeCreationOGLTimer", 1)

      const sh::ComputeShader& shader = m_shaderContainer->getComputeShader(m_calculateAreaLightTube);
      shader.useShader();

      indirectLightPositions->setTexture(0, 0);

      sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);

      m_proxyLightTubeBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      sh::ComputeShader::dispatchComputeShader(indirectLightPositions->getResolution()[2], 1, 1);

      m_proxyLightTubeBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      indirectLightPositions->unsetTexture(0);

      shader.useNoShader();

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<util::vec4f> tubeData(m_reflectiveShadowMapNumber * 2);
      //m_proxyLightTubeBuffer.getData(0, m_reflectiveShadowMapNumber * 2 * sizeof(util::vec4f), &tubeData[0]);
    }

    void IndirectSpecularReflections::createSpecularInnerCaches(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap)
    {
      //CPUTIMER("InnerCacheCreationCPUTimer", 0)
      //GPUTIMER("InnerCacheCreationOGLTimer", 1)

      float clearValue = 0.0f;
      m_cachePositionBufferInnerX->clearTexture(&clearValue);

      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      util::vec2i stepSize[2];
      stepSize[0] = util::vec2i(1, 0);
      stepSize[1] = util::vec2i(0, 1);

      m_proxyLightTubeBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      const sh::ComputeShader& specularCachePositionFilterXShader = m_shaderContainer->getComputeShader(m_specularCachePositionFilterX);
      specularCachePositionFilterXShader.useShader();

      m_cachePositionBufferInnerX->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R16F);

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);

      sh::ComputeShader::setUniform(3, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);
      sh::ComputeShader::setUniform(4, GL_INT_VEC2, &stepSize[0][0]);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_cachePositionBufferInnerX->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R16F);

      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      specularCachePositionFilterXShader.useNoShader();

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

      m_cacheOffsetMap->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);
      m_cachePositionMap->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_cacheNormalMap->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_cacheAtomicIndexMap->bindImageTexture(3, 0, GL_READ_WRITE, GL_R32UI);

      m_vertexPositionMap->bindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(5, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexAtomicIndexMap->bindImageTexture(6, 0, GL_READ_WRITE, GL_R32UI);

      const sh::ComputeShader& specularCachePositionFilterYShader = m_shaderContainer->getComputeShader(m_specularCachePositionFilterY);
      specularCachePositionFilterYShader.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);

      m_cachePositionBufferInnerX->setTexture(3, 3);

      sh::ComputeShader::setUniform(4, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);
      sh::ComputeShader::setUniform(5, GL_INT_VEC2, &stepSize[1][0]);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_cachePositionBufferInnerX->unsetTexture(3);

      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      specularCachePositionFilterYShader.useNoShader();

      m_vertexAtomicIndexMap->unbindImageTexture(6, 0, GL_READ_WRITE, GL_R32UI);
      m_vertexNormalMap->unbindImageTexture(5, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(4, 0, GL_WRITE_ONLY, GL_RGBA32F);

      m_cacheAtomicIndexMap->unbindImageTexture(3, 0, GL_READ_WRITE, GL_R32UI);
      m_cacheNormalMap->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_cachePositionMap->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_cacheOffsetMap->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);

      m_proxyLightTubeBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    }

    void IndirectSpecularReflections::createSpecularEdgeCaches(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap,
      util::SharedPointer<db::Texture2D> gBufferLinearDepthMap)
    {
      //CPUTIMER("EdgeCacheCreationCPUTimer", 0)
      //GPUTIMER("EdgeCacheCreationOGLTimer", 1)

      float clearValue = 0.0f;
      m_cacheOffsetMap->clearTexture(&clearValue);

      unsigned int clearIntValue = 0;
      m_cacheAtomicIndexMap->clearTexture(&clearIntValue);

      util::vec4f clearVectorVal = util::vec4f::identity();
      m_cachePositionMap->clearTexture(&clearVectorVal[0]);
      m_cacheNormalMap->clearTexture(&clearVectorVal[0]);
      

      const sh::ComputeShader& specularCacheEdgeCacheCreationShader = m_shaderContainer->getComputeShader(m_specularCacheEdgeCacheCreation);
      specularCacheEdgeCacheCreationShader.useShader();

      m_proxyLightTubeBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_cacheOffsetMap->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);
      m_cachePositionMap->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_cacheNormalMap->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_cacheAtomicIndexMap->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_R32UI);

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);
      gBufferLinearDepthMap->setTexture(3, 3);

      sh::ComputeShader::dispatchComputeShader(2048, 1, 1);

      gBufferLinearDepthMap->unsetTexture(3);
      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      m_cacheAtomicIndexMap->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_R32UI);
      m_cacheNormalMap->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_cachePositionMap->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_cacheOffsetMap->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);

      m_proxyLightTubeBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      specularCacheEdgeCacheCreationShader.useNoShader();
    }

    void IndirectSpecularReflections::createEdgeVertices(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap,
      util::SharedPointer<db::Texture2D> gBufferLinearDepthMap)
    {
      //CPUTIMER("CreateEdgeVertexCPUTimer", 0)
      //GPUTIMER("CreateEdgeVertexOGLTimer", 1)

      unsigned int clearIntValue = 0;
      m_vertexAtomicIndexMap->clearTexture(&clearIntValue);

      util::vec4f clearVectorVal = util::vec4f::identity();
      m_vertexPositionMap->clearTexture(&clearVectorVal[0]);
      m_vertexNormalMap->clearTexture(&clearVectorVal[0]);

      const sh::ComputeShader& specularCacheEdgeVertexCreationShader = m_shaderContainer->getComputeShader(m_specularCacheEdgeVertexCreation);
      specularCacheEdgeVertexCreationShader.useShader();

      m_proxyLightTubeBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_vertexPositionMap->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexAtomicIndexMap->bindImageTexture(2, 0, GL_READ_WRITE, GL_R32UI);

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);
      gBufferLinearDepthMap->setTexture(3, 3);

      sh::ComputeShader::dispatchComputeShader(2048, 1, 1);

      gBufferLinearDepthMap->unsetTexture(3);
      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      m_vertexAtomicIndexMap->unbindImageTexture(2, 0, GL_READ_WRITE, GL_R32UI);
      m_vertexNormalMap->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA32F);

      m_proxyLightTubeBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      specularCacheEdgeVertexCreationShader.useNoShader();
    }

    void IndirectSpecularReflections::createOffsets(util::SharedPointer<db::Texture2D> offsetMap)
    {
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      const sh::ComputeShader& specularCacheOffsetUpSweepShader = m_shaderContainer->getComputeShader(m_specularCacheOffsetUpSweep);
      specularCacheOffsetUpSweepShader.useShader();

      unsigned int pixelNumber = offsetMap->getResolution()[0] * offsetMap->getResolution()[1];
      unsigned int workGroupSize = 128;
      const unsigned int maxWorkGroupNumber = 128;
      unsigned int workGroupNumber;
      unsigned int pixelStride = 1;

      do
      {
        workGroupNumber = std::min<unsigned int>(ceil(pixelNumber / float(pixelStride * 2 * workGroupSize)), maxWorkGroupNumber);

        offsetMap->bindImageTexture(0, 0, GL_READ_WRITE, GL_R32F);

        sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &pixelStride);

        sh::ComputeShader::dispatchComputeShader(workGroupNumber, 1, 1);

        offsetMap->unbindImageTexture(0, 0, GL_READ_WRITE, GL_R32F);

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
        workGroupNumber = std::min<unsigned int>(ceil(pixelNumber / float(actualSampleRangePerWorkGroup)), maxWorkGroupNumber);
        unsigned int pixelPerThread = ceil(roundedPixelNumber / float(pixelStride * workGroupNumber * workGroupSize));

        offsetMap->bindImageTexture(0, 0, GL_READ_WRITE, GL_R32F);

        sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &pixelStride);
        sh::ComputeShader::setUniform(1, GL_UNSIGNED_INT, &actualSampleRangePerWorkGroup);
        sh::ComputeShader::setUniform(2, GL_UNSIGNED_INT, &pixelPerThread);

        sh::ComputeShader::dispatchComputeShader(workGroupNumber, 1, 1);

        offsetMap->unbindImageTexture(0, 0, GL_READ_WRITE, GL_R32F);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        actualSampleRangePerWorkGroup = pixelStride;
      } while(pixelStride > 1);

      specularCacheOffsetDownSweepShader.useNoShader();

      //std::vector<float> offsets(pixelNumber);
      //offsetMap->getTextureData(&offsets[0]);
      //m_debugCacheNumber = offsets[pixelNumber - 1];
      //unsigned int endVal1 = offsets[65535];
      //unsigned int endVal2 = offsets[131071];
      //unsigned int endVal3 = offsets[196607];

      //std::vector<float> offsets0(offsetMap->getResolution()[0] * 16 * 3);
      //for(unsigned int i = 0; i < offsets0.size(); i++)
      //{
      //  unsigned int index = offsetMap->getResolution()[0] * 16 * 26;
      //  if(index + i < offsets.size())
      //  {
      //    offsets0[i] = offsets[index + i];
      //  }
      //}
    }

    void IndirectSpecularReflections::createSpecularCacheBuffer()
    {
      //CPUTIMER("CacheBufferCreationCPUTimer", 0)
      //GPUTIMER("CacheBufferCreationOGLTimer", 1)

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      const sh::ComputeShader& specularCacheCreationShader = m_shaderContainer->getComputeShader(m_specularCacheBufferCreation);
      specularCacheCreationShader.useShader();

      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      m_cacheOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);
      m_cachePositionMap->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cacheNormalMap->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_cacheNormalMap->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cachePositionMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cacheOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      specularCacheCreationShader.useNoShader();


      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<util::vec4f> cachePos(m_options->specularCacheNumber);
      //m_cachePositions.getData(0, m_options->specularCacheNumber * sizeof(util::vec4f), &cachePos[0]);

      //std::vector<util::vec4f> cacheNormalMat(m_options->specularCacheNumber);
      //m_cacheNormalMaterial.getData(0, m_options->specularCacheNumber * sizeof(util::vec4f), &cacheNormalMat[0]);
    }

    void IndirectSpecularReflections::createProxyLightBuffer(
      util::SharedPointer<db::Texture3D> indirectLightPositions,
      util::SharedPointer<db::Texture3D> indirectLightNormals,
      util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
      util::SharedPointer<db::Texture3D> indirectShadowMaps,
      util::SharedPointer<db::Texture2D> indirectShadowVALQuaternions)
    {
      //CPUTIMER("ProxyLightCPUTimer", 0)
      //GPUTIMER("ProxyLightOGLTimer", 1)

      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      const sh::ComputeShader& specularProxyLightCreationShader = m_shaderContainer->getComputeShader(m_specularProxyLightBufferCreation);
      specularProxyLightCreationShader.useShader();

      indirectLightPositions->setTexture(0, 0);
      indirectLightNormals->setTexture(1, 1);
      indirectLightLuminousFlux->setTexture(2, 2);
      indirectShadowMaps->setTexture(3, 3);
      indirectShadowVALQuaternions->setTexture(4, 4);

      sh::ComputeShader::setUniform(5, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);
      sh::ComputeShader::setUniform(6, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);
      sh::ComputeShader::setUniform(7, GL_UNSIGNED_INT, &m_options->indirectShadowMapWidth);
      sh::ComputeShader::setUniform(8, GL_UNSIGNED_INT, &m_options->giShadowLightSampleDivisor);

      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_proxyLightPositionBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_proxyLightLuminousFluxBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

      m_cacheOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      sh::ComputeShader::dispatchComputeShader(1024, 1, 1);

      m_cacheOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      m_proxyLightLuminousFluxBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_proxyLightPositionBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      indirectShadowVALQuaternions->unsetTexture(4);
      indirectShadowMaps->unsetTexture(3);
      indirectLightLuminousFlux->unsetTexture(2);
      indirectLightNormals->unsetTexture(1);
      indirectLightPositions->unsetTexture(0);

      specularProxyLightCreationShader.useNoShader();
      
      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<util::vec4f> proxyLightPositionData(2 * m_options->specularCacheNumber);
      //m_proxyLightPositionBuffer.getData(0, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), &proxyLightPositionData[0]);

      //std::vector<util::vec4f> proxyLightLuminousFluxData(2 * m_options->specularCacheNumber);
      //m_proxyLightLuminousFluxBuffer.getData(0, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), &proxyLightLuminousFluxData[0]);

      //std::vector<util::vec4f> proxyLightNormalData(2 * m_options->specularCacheNumber);
      //m_proxyLightNormalBuffer.getData(0, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), &proxyLightNormalData[0]);
    }

    void IndirectSpecularReflections::createSpecularCacheIndices()
    {
      //CPUTIMER("CacheIndexCPUTimer", 0)
      //GPUTIMER("CacheIndexOGLTimer", 1)

      util::vec4ui clearVector(INT32_MAX);
      m_cacheIndexBuffer.clearBuffer(GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT, &clearVector[0]);

      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      const sh::ComputeShader& specularCacheIndexListGenerationShader = m_shaderContainer->getComputeShader(m_specularCacheIndexListGeneration);
      specularCacheIndexListGenerationShader.useShader();

      m_cacheOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);
      m_cachePositionMap->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cacheNormalMap->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cacheAtomicIndexMap->bindImageTexture(3, 0, GL_READ_ONLY, GL_R32UI);

      m_vertexPositionMap->bindImageTexture(4, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexAtomicIndexMap->bindImageTexture(6, 0, GL_READ_ONLY, GL_R32UI);

      m_cacheIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      sh::ComputeShader::dispatchComputeShader(4096, 1, 1);

      m_cacheIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_vertexAtomicIndexMap->unbindImageTexture(6, 0, GL_READ_ONLY, GL_R32UI);
      m_vertexNormalMap->unbindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(4, 0, GL_READ_ONLY, GL_RGBA32F);

      m_cacheAtomicIndexMap->unbindImageTexture(3, 0, GL_READ_ONLY, GL_R32UI);
      m_cacheNormalMap->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cachePositionMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cacheOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      specularCacheIndexListGenerationShader.useNoShader();

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<unsigned int> cacheIndices0(8 * m_indirectLightResolution[0] * m_indirectLightResolution[1]);
      //m_cacheIndexBuffer.getData(0, 8 * m_indirectLightResolution[0] * m_indirectLightResolution[1] * sizeof(unsigned int), &cacheIndices0[0]);

      //std::vector<unsigned int> cacheIndices1(8 * m_indirectLightResolution[0] * m_indirectLightResolution[1]);
      //m_cacheIndexBuffer.getData(8 * m_indirectLightResolution[0] * 416 * sizeof(unsigned int), 8 * m_indirectLightResolution[0] * 34 * sizeof(unsigned int), &cacheIndices1[0]);
    }

      void IndirectSpecularReflections::createindirectLightMap(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap)
    {
      //CPUTIMER("IndirectLightMapCreationCPUTimer", 0)
      //GPUTIMER("IndirectLightMapCreationOGLTimer", 1)

      util::vec4f clearValue = util::vec4f(0.0f);
      m_specularLightMap->clearTexture(&clearValue[0]);

      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      const sh::ComputeShader& indirectLightMapCreation = m_shaderContainer->getComputeShader(m_indirectLightMapCreation);
      indirectLightMapCreation.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);

      m_specularLightMap->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA16F);
      m_vertexAtomicIndexMap->bindImageTexture(1, 0, GL_READ_ONLY, GL_R32UI);
      m_vertexPositionMap->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
      
      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_proxyLightPositionBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_proxyLightLuminousFluxBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_cacheIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 4);

      sh::ComputeShader::dispatchComputeShader(4096, 1, 1);

      m_cacheIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_proxyLightLuminousFluxBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_proxyLightPositionBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_vertexNormalMap->unbindImageTexture(3, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexAtomicIndexMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R32UI);
      m_specularLightMap->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA16F);

      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      indirectLightMapCreation.useNoShader();
    }

    void IndirectSpecularReflections::indirectLightMapUpsampling()
    {
      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

      const sh::ComputeShader& indirectLightMapUpsampling = m_shaderContainer->getComputeShader(m_indirectLightMapUpsampling);
      indirectLightMapUpsampling.useShader();

      sh::ComputeShader::setUniform(0, GL_INT_VEC2, &m_indirectLightResolution[0]);

      util::vec2i frameBufferResolution(m_options->width, m_options->height);
      sh::ComputeShader::setUniform(1, GL_INT_VEC2, &frameBufferResolution[0]);

      unsigned int samplingRatio = frameBufferResolution[0] / m_indirectLightResolution[0];
      sh::ComputeShader::setUniform(2, GL_UNSIGNED_INT, &samplingRatio);

      m_specularLightMap->setTexture(3, 3);

      m_upsampledSpecularLightMap->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA16F);

      sh::ComputeShader::dispatchComputeShader(4096, 1, 1);
      
      m_upsampledSpecularLightMap->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_RGBA16F);

      m_specularLightMap->unsetTexture(3);

      indirectLightMapUpsampling.useNoShader();

      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);//barrier is for reading the texture by a sampler in any other shader
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugCachePositionsMapFilterInnerX() const
    {
      return m_cachePositionBufferInnerX;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getSpecularLightMap() const
    {
      return m_specularLightMap; 
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getUpsampledSpecularLightMap() const
    {
      return m_upsampledSpecularLightMap;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugCachePositions() const
    {
      return m_cachePositionMap;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugCacheNormals() const
    {
      return m_cacheNormalMap;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugVertexPositions() const
    {
      return m_vertexPositionMap;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugVertexNormals() const
    {
      return m_vertexNormalMap;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugVertexBlockNumber() const
    {
      return m_vertexAtomicIndexMap;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugCacheBlockNumber() const
    {
      return m_cacheAtomicIndexMap;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugGBufferHalfResDepthMap() const
    {
      return m_halfResGBufferDepthMap1;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugGBufferHalfResLinearDepthMap() const
    {
      return m_halfResGBufferLinearDepthMap1;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugGBufferHalfResNormalMap() const
    {
      return m_halfResGBufferNormalMap1;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugGBufferHalfResVertexNormalMap() const
    {
      return m_halfResGBufferVertexNormalMap1;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugGBufferHalfResMaterialMap() const
    {
      return m_halfResGBufferMaterialMap1;
    }
  }
}
