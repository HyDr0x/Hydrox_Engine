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

    IndirectSpecularReflections::IndirectSpecularReflections() : m_vertexResolution(16), m_cacheResolution(32), m_ping(0), m_pong(1 - m_ping), m_proxyLightPing(0), m_proxyLightPong(1 - m_proxyLightPing)
    {
      glGenVertexArrays(1, &m_triangleVAO);
      glGenVertexArrays(1, &m_simpleTriangleVAO);
    }

    IndirectSpecularReflections::~IndirectSpecularReflections()
    {
      glDeleteVertexArrays(1, &m_triangleVAO);
      glDeleteVertexArrays(1, &m_simpleTriangleVAO);
    }

    void IndirectSpecularReflections::initialize(util::SingletonManager *singletonManager)
    {
      glBindVertexArray(m_simpleTriangleVAO);
      glVertexAttribFormat(sh::RenderShader::POSITION, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::POSITION, 0);
      glBindVertexArray(0);

      glBindVertexArray(m_triangleVAO);
      glVertexAttribFormat(sh::RenderShader::POSITION, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::POSITION, 0);
      glVertexAttribFormat(sh::RenderShader::NORMAL, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::NORMAL, 1);
      glVertexAttribFormat(sh::RenderShader::CACHEINDICES0, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::CACHEINDICES0, 2);
      glVertexAttribFormat(sh::RenderShader::CACHEINDICES1, 4, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::CACHEINDICES1, 3);
      glBindVertexArray(0);

      m_options = singletonManager->getService<RenderOptions>();

      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_calculateAreaLightTube = sh::ShaderContainer::SPECULARTUBECREATION;
      m_specularCachePositionFilterX = sh::ShaderContainer::SPECULARCACHEPOSITIONFILTERX;
      m_specularCachePositionFilterY = sh::ShaderContainer::SPECULARCACHEPOSITIONFILTERY;
      m_specularCacheOffsetUpSweep = sh::ShaderContainer::SPECULARCACHEOFFSETUPSWEEP;
      m_specularCacheOffsetDownSweep = sh::ShaderContainer::SPECULARCACHEOFFSETDOWNSWEEP;
      m_specularCacheBufferCreation = sh::ShaderContainer::SPECULARCACHECREATION;
      m_specularProxyLightBufferCreation = sh::ShaderContainer::SPECULARPROXYLIGHTCREATION;
      m_specularProxyLightBufferCreationDivide = sh::ShaderContainer::SPECULARPROXYLIGHTBUFFERCREATIONDIVIDE;
      m_specularCacheVertexBufferCreation = sh::ShaderContainer::SPECULARCACHEVERTEXBUFFERCREATION;
      m_specularCacheBoundingBoxComputation = sh::ShaderContainer::SPECULARCACHEBOUNDINGBOXCOMPUTATION;
      m_specularCacheIndexListGeneration = sh::ShaderContainer::SPECULARCACHEINDEXLISTCREATION;
      m_specularCacheTriangleIndexOffsetCalculation = sh::ShaderContainer::SPECULARCACHETRIANGLEINDEXOFFSETCALCULATION;
      m_specularCacheFillTriangleIndices = sh::ShaderContainer::SPECULARCACHEFILLTRIANGLEINDICES;
      m_specularCacheEdgeCacheCreation = sh::ShaderContainer::SPECULARCACHEEDGECACHECREATION;
      m_specularCacheVoronoiDiagramCornerRepair = sh::ShaderContainer::SPECULARCACHEVORONOIDIAGRAMCORNERREPAIR;
      m_specularCacheEdgeVertexCreation = sh::ShaderContainer::SPECULARCACHEEDGEVERTEXCREATION;
      m_specularDebugCacheVertexIndexBufferCreation = sh::ShaderContainer::SPECULARDEBUGCACHEVERTEXINDEXBUFFERCREATION;
      m_specularCacheEdgeVertexExtraction = sh::ShaderContainer::SPECULARCACHEEDGEVERTEXEXTRACTION;

      m_showLines = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SHOWLINES, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION));
      m_specularJumpFloodAlgorithm = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SPECULARJUMPFLOODALGORITHM, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_specularJumpFloodIslandRemovalAlgorithm = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SPECULARJUMPFLOODISLANDREMOVALALGORITHM, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_showVoronoiDiagram = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SHOWVORONOIDIAGRAM, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_showSpecularCacheDelaunayTriangulation = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SHOWSPECULARCACHEDELAUNAYTRIANGULATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION));
      m_specularLightMapDelaunayTriangulationCreation = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SPECULARCACHEPROXYLIGHTMAPDELAUNAYTRIANGULATION, 
        sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION) | sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::NORMAL) | sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::CACHEINDICES0) | sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::CACHEINDICES1));
      m_indirectSpecularErrorSpotLighting = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::INDIRECTSPECULARERRORSPOTLIGHTING, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_proxyLightWeightCreation = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::PROXYLIGHTWEIGHTCREATION, sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::SPECIAL1));
      m_indirectLightScreeVertexExtraction = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::INDIRECTLIGHTSCREENVERTEXEXTRACTION,
        sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::POSITION) | sh::ShaderSlotFlags::convertToFlag(sh::RenderShader::NORMAL));

      m_cachePositionBufferInnerX = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R16F, GL_RED, 1, 16));

      m_cacheAtomicIndexMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width / m_cacheResolution, m_options->height / m_cacheResolution, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R32UI, GL_RED_INTEGER, 1, 32));
      m_cacheOffsetMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_cachePositionMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_cacheNormalMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));

      m_vertexAtomicIndexMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width / m_vertexResolution, m_options->height / m_vertexResolution, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R32UI, GL_RED_INTEGER, 1, 32));
      m_vertexOffsetMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_vertexPositionMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_vertexNormalMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_edgeVertexPositionMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));
      m_edgeVertexNormalMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA32F, GL_RGBA, 4, 32));

      m_triangleIndexOffsets = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));

      m_voronoiDiagram.resize(2);
      m_voronoiDiagram[m_ping] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R16UI, GL_RED_INTEGER, 1, 16));
      m_voronoiDiagram[m_pong] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_R16UI, GL_RED_INTEGER, 1, 16));

      m_cacheIndices.resize(6);
      for(unsigned int i = 0; i < 6; i++)
      {
        m_cacheIndices[i] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_UNSIGNED_INT, GL_RGBA16UI, GL_RGBA_INTEGER, 4, 16));
      }

      m_proxyLightPositionMap.resize(2);
      m_proxyLightLuminousFluxMap.resize(2);
      unsigned int proxyLightTextureResolution = sqrtf(float(m_options->specularCacheNumber));

      m_proxyLightPositionMap[m_proxyLightPing] = util::SharedPointer<db::Texture2D>(new db::Texture2D(proxyLightTextureResolution, proxyLightTextureResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_proxyLightPositionMap[m_proxyLightPong] = util::SharedPointer<db::Texture2D>(new db::Texture2D(proxyLightTextureResolution, proxyLightTextureResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));

      m_proxyLightLuminousFluxMap[m_proxyLightPing] = util::SharedPointer<db::Texture2D>(new db::Texture2D(proxyLightTextureResolution, proxyLightTextureResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_proxyLightLuminousFluxMap[m_proxyLightPong] = util::SharedPointer<db::Texture2D>(new db::Texture2D(proxyLightTextureResolution, proxyLightTextureResolution, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));

      m_specularLightMap = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_specularProxyLightQuad.setRenderTargets(1, m_specularLightMap);

      m_proxyLightPositionBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), 2 * m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_proxyLightLuminousFluxBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), 2 * m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      
      m_cachePositions.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_cacheNormalMaterial.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);

      m_projectedVertexPositions.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec2i), m_options->specularCacheNumber * sizeof(util::vec2i), GL_STATIC_DRAW);
      m_vertexPositions.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_vertexNormals.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);

      //a simple planar crossing-free embedding of a graph G with N >= 3 vertices has at most 2 * N - 4 faces (from Euler Formula)
      m_triangleIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 3 * (2 * m_options->specularCacheNumber - 4) * sizeof(unsigned int), 3 * (2 * m_options->specularCacheNumber - 4) * sizeof(unsigned int), GL_STATIC_DRAW);

      m_specularCacheIndexBuffer0.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);
      m_specularCacheIndexBuffer1.createBuffer(GL_SHADER_STORAGE_BUFFER, m_options->specularCacheNumber * sizeof(util::vec4f), m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);

      m_cacheBBBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), 2 * m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);

      m_debugCacheVertexIndexPositions.createBuffer(GL_ARRAY_BUFFER, 16 * m_options->specularCacheNumber * sizeof(util::vec4f), 16 * m_options->specularCacheNumber * sizeof(util::vec4f), GL_STATIC_DRAW);

      DrawElementsIndirectCommand drawElementCommand;
      drawElementCommand.baseInstance = 0;
      drawElementCommand.baseVertex = 0;
      drawElementCommand.first = 0;
      drawElementCommand.primCount = 1;
      m_triangleCommandBuffer.createBuffer(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), sizeof(DrawElementsIndirectCommand), GL_STATIC_DRAW, &drawElementCommand);

      m_borderVertexNumber = 2 * (m_options->width + m_options->height) - 4;//-4 because the width and height resolution are sharing the same corner pixels
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
      util::SharedPointer<db::Texture3D> indirectLightLuminousFlux)
    {
      CPUTIMER("MainloopCPUTimer", 0)
      GPUTIMER("MainloopOGLTimer", 1)

      createTubeData(indirectLightPositions);

      createSpecularEdgeCaches(gBufferDepthMap, gBufferVertexNormalMap, gBufferMaterialMap, gBufferLinearDepthMap);
      createEdgeVertices(gBufferDepthMap, gBufferVertexNormalMap, gBufferMaterialMap, gBufferLinearDepthMap);
      createSpecularInnerCaches(gBufferDepthMap, gBufferVertexNormalMap, gBufferMaterialMap);

      createOffsets(m_cacheOffsetMap);
      createOffsets(m_vertexOffsetMap);

      createSpecularCacheBuffer();

      createProxyLightBuffer(indirectLightPositions, indirectLightNormals, indirectLightLuminousFlux);

      createVertexBuffer();

      createVoronoiDiagram(m_projectedVertexPositions);

      calculateTriangleIndexOffsets();

      fillTriangleIndexBuffer();

      createSpecularCacheIndices();

      createTriangleSpecularProxyLightMap(gBufferDepthMap, gBufferNormalMap, gBufferMaterialMap, gBufferLinearDepthMap);

      specularErrorSpotLight(gBufferDepthMap, gBufferNormalMap, gBufferMaterialMap);
    }

    void IndirectSpecularReflections::setEdgeVertexMap(util::SharedPointer<db::Texture2D> gBufferDepthMap)
    {
      glDepthMask(GL_FALSE);
      glDepthFunc(GL_LEQUAL);

      util::vec4f clearVector(0.0f);
      m_edgeVertexPositionMap->clearTexture(&clearVector[0]);
      m_edgeVertexNormalMap->clearTexture(&clearVector[0]);

      m_edgeVertexRenderQuad.setRenderTargets(gBufferDepthMap, 2, m_edgeVertexPositionMap, m_edgeVertexNormalMap);
      m_edgeVertexRenderQuad.setWriteFrameBuffer();
    }

    void IndirectSpecularReflections::unsetEdgeVertexMap()
    {
      m_edgeVertexRenderQuad.unsetWriteFrameBuffer();

      glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);
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

      m_vertexOffsetMap->bindImageTexture(4, 0, GL_WRITE_ONLY, GL_R32F);
      m_vertexPositionMap->bindImageTexture(5, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(6, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexAtomicIndexMap->bindImageTexture(7, 0, GL_READ_WRITE, GL_R32UI);

      const sh::ComputeShader& specularCachePositionFilterYShader = m_shaderContainer->getComputeShader(m_specularCachePositionFilterY);
      specularCachePositionFilterYShader.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);

      m_cachePositionBufferInnerX->setTexture(3, 3);

      sh::ComputeShader::setUniform(4, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);
      sh::ComputeShader::setUniform(5, GL_INT_VEC2, &stepSize[1][0]);
      sh::ComputeShader::setUniform(6, GL_UNSIGNED_INT, &m_cacheResolution);
      sh::ComputeShader::setUniform(7, GL_UNSIGNED_INT, &m_vertexResolution);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_cachePositionBufferInnerX->unsetTexture(3);

      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      specularCachePositionFilterYShader.useNoShader();

      m_vertexAtomicIndexMap->unbindImageTexture(7, 0, GL_READ_WRITE, GL_R32UI);
      m_vertexNormalMap->unbindImageTexture(6, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(5, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexOffsetMap->unbindImageTexture(4, 0, GL_WRITE_ONLY, GL_R32F);

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

      float clearValue = 0.0f;
      m_vertexOffsetMap->clearTexture(&clearValue);

      unsigned int clearIntValue = 0;
      m_vertexAtomicIndexMap->clearTexture(&clearIntValue);

      util::vec4f clearVectorVal = util::vec4f::identity();
      m_vertexPositionMap->clearTexture(&clearVectorVal[0]);
      m_vertexNormalMap->clearTexture(&clearVectorVal[0]);
      /*
      const sh::ComputeShader& specularCacheEdgeVertexExtraction = m_shaderContainer->getComputeShader(m_specularCacheEdgeVertexExtraction);
      specularCacheEdgeVertexExtraction.useShader();

      m_proxyLightTubeBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_vertexOffsetMap->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);
      m_vertexPositionMap->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexAtomicIndexMap->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_R32UI);

      m_edgeVertexPositionMap->setTexture(0, 0);
      m_edgeVertexNormalMap->setTexture(1, 1);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_edgeVertexNormalMap->unsetTexture(1);
      m_edgeVertexPositionMap->unsetTexture(0);

      m_vertexAtomicIndexMap->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_R32UI);
      m_vertexNormalMap->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexOffsetMap->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);

      m_proxyLightTubeBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      specularCacheEdgeVertexExtraction.useNoShader();

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      */
      const sh::ComputeShader& specularCacheEdgeVertexCreationShader = m_shaderContainer->getComputeShader(m_specularCacheEdgeVertexCreation);
      specularCacheEdgeVertexCreationShader.useShader();

      m_proxyLightTubeBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_vertexOffsetMap->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);
      m_vertexPositionMap->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexAtomicIndexMap->bindImageTexture(3, 0, GL_READ_WRITE, GL_R32UI);

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);
      gBufferLinearDepthMap->setTexture(3, 3);

      sh::ComputeShader::dispatchComputeShader(2048, 1, 1);

      gBufferLinearDepthMap->unsetTexture(3);
      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      m_vertexAtomicIndexMap->unbindImageTexture(3, 0, GL_READ_WRITE, GL_R32UI);
      m_vertexNormalMap->unbindImageTexture(2, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_RGBA32F);
      m_vertexOffsetMap->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);

      m_proxyLightTubeBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      specularCacheEdgeVertexCreationShader.useNoShader();
    }

    void IndirectSpecularReflections::createOffsets(util::SharedPointer<db::Texture2D> offsetMap)
    {
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      const sh::ComputeShader& specularCacheOffsetUpSweepShader = m_shaderContainer->getComputeShader(m_specularCacheOffsetUpSweep);
      specularCacheOffsetUpSweepShader.useShader();

      unsigned int pixelNumber = m_options->width * m_options->height;
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

      //std::vector<float> offsets(m_options->width * m_options->height);
      //offsetMap->getTextureData(&offsets[0]);
      //m_debugCacheNumber = offsets[m_options->width * m_options->height - 1];
      //unsigned int endVal1 = offsets[65535];
      //unsigned int endVal2 = offsets[131071];
      //unsigned int endVal3 = offsets[196607];
      //unsigned int endVal4 = offsets[8 * 65536 - 1];
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
      util::SharedPointer<db::Texture3D> indirectLightLuminousFlux)
    {
      //CPUTIMER("ProxyLightCPUTimer", 0)
      //GPUTIMER("ProxyLightOGLTimer", 1)

      /*
      util::vec4f clearValue = util::vec4f(0.0f);
      m_proxyLightPositionMap[m_proxyLightPing]->clearTexture(&clearValue);
      m_proxyLightLuminousFluxMap[m_proxyLightPing]->clearTexture(&clearValue);
      m_proxyLightPositionMap[m_proxyLightPong]->clearTexture(&clearValue);
      m_proxyLightLuminousFluxMap[m_proxyLightPong]->clearTexture(&clearValue);

      unsigned int cacheSize = 128;
      unsigned int cacheRounds = (m_options->giLightSampleNumber + cacheSize - 1) / cacheSize;
      
      glViewport(0, 0, m_proxyLightPositionMap[m_proxyLightPing]->getResolution()[0], m_proxyLightPositionMap[m_proxyLightPing]->getResolution()[1]);

      const sh::RenderShader& specularProxyLightCreationShader = m_shaderContainer->getRenderShader(m_proxyLightWeightCreation);
      specularProxyLightCreationShader.useShader();

      indirectLightPositions->setTexture(0, 0);
      indirectLightNormals->setTexture(1, 1);
      indirectLightLuminousFlux->setTexture(2, 2);

      sh::RenderShader::setUniform(3, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);
      sh::RenderShader::setUniform(4, GL_UNSIGNED_INT, &cacheSize);

      samplingPatternBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

      m_cacheOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      for(unsigned int i = 0; i < cacheRounds; i++)
      {
        m_proxyLightAccumulationQuad.setRenderTargets(2, m_proxyLightPositionMap[m_proxyLightPing], m_proxyLightLuminousFluxMap[m_proxyLightPing]);
        m_proxyLightAccumulationQuad.setWriteFrameBuffer();

        m_proxyLightPositionMap[m_proxyLightPong]->setTexture(5, 5);
        m_proxyLightLuminousFluxMap[m_proxyLightPong]->setTexture(6, 6);

        unsigned int valOffset = i * cacheSize;
        sh::RenderShader::setUniform(7, GL_UNSIGNED_INT, &valOffset);
        m_proxyLightAccumulationQuad.render();

        m_proxyLightPositionMap[m_proxyLightPong]->unsetTexture(5);
        m_proxyLightLuminousFluxMap[m_proxyLightPong]->unsetTexture(6);

        m_proxyLightAccumulationQuad.unsetWriteFrameBuffer();

        m_proxyLightPong = m_proxyLightPing;
        m_proxyLightPing = 1 - m_proxyLightPing;

        //std::vector<util::vec4f> proxyLightPositionData(m_options->specularCacheNumber);
        //m_proxyLightPositionMap[m_proxyLightPong]->getTextureData(&proxyLightPositionData[0]);

        //std::vector<util::vec4f> proxyLightLuminousFluxData(m_options->specularCacheNumber);
        //m_proxyLightLuminousFluxMap[m_proxyLightPong]->getTextureData(&proxyLightLuminousFluxData[0]);
      }

      indirectLightLuminousFlux->unsetTexture(2);
      indirectLightNormals->unsetTexture(1);
      indirectLightPositions->unsetTexture(0);

      specularProxyLightCreationShader.useNoShader();

      glViewport(0, 0, m_options->width, m_options->height);

      m_proxyLightPositionBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_proxyLightLuminousFluxBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 4);

      const sh::ComputeShader& specularProxyLightCreationDivideShader = m_shaderContainer->getComputeShader(m_specularProxyLightBufferCreationDivide);
      specularProxyLightCreationDivideShader.useShader();
      
      m_proxyLightPositionMap[m_proxyLightPong]->setTexture(0, 0);
      m_proxyLightLuminousFluxMap[m_proxyLightPong]->setTexture(1, 1);

      sh::ComputeShader::setUniform(2, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_proxyLightLuminousFluxMap[m_proxyLightPong]->unsetTexture(1);
      m_proxyLightPositionMap[m_proxyLightPong]->unsetTexture(0);

      specularProxyLightCreationDivideShader.useNoShader();

      m_cacheOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      m_proxyLightLuminousFluxBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_proxyLightPositionBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      samplingPatternBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<util::vec4f> proxyLightPositionData(m_options->specularCacheNumber);
      //m_proxyLightPositionBuffer.getData(0, m_options->specularCacheNumber * sizeof(util::vec4f), &proxyLightPositionData[0]);

      //std::vector<util::vec4f> proxyLightLuminousFluxData(m_options->specularCacheNumber);
      //m_proxyLightLuminousFluxBuffer.getData(0, m_options->specularCacheNumber * sizeof(util::vec4f), &proxyLightLuminousFluxData[0]);
      */

      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      const sh::ComputeShader& specularProxyLightCreationShader = m_shaderContainer->getComputeShader(m_specularProxyLightBufferCreation);
      specularProxyLightCreationShader.useShader();

      indirectLightPositions->setTexture(0, 0);
      indirectLightNormals->setTexture(1, 1);
      indirectLightLuminousFlux->setTexture(2, 2);

      sh::ComputeShader::setUniform(3, GL_UNSIGNED_INT, &m_reflectiveShadowMapNumber);
      sh::ComputeShader::setUniform(4, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);

      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_proxyLightPositionBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_proxyLightLuminousFluxBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      
      m_cacheOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_cacheOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      m_proxyLightLuminousFluxBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_proxyLightPositionBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

    void IndirectSpecularReflections::createVertexBuffer()
    {
      //CPUTIMER("VertexBufferCreationCPUTimer", 0)
      //GPUTIMER("VertexBufferCreationOGLTimer", 1)

      unsigned int clearVal = UINT16_MAX;
      m_voronoiDiagram[m_pong]->clearTexture(&clearVal);
      m_voronoiDiagram[m_ping]->clearTexture(&clearVal);

      const sh::ComputeShader& vertexBufferCreationShader = m_shaderContainer->getComputeShader(m_specularCacheVertexBufferCreation);
      vertexBufferCreationShader.useShader();

      sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &m_borderVertexNumber);

      m_projectedVertexPositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_vertexPositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_vertexNormals.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      
      m_vertexOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);
      m_vertexPositionMap->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_voronoiDiagram[m_pong]->bindImageTexture(3, 0, GL_WRITE_ONLY, GL_R16UI);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_voronoiDiagram[m_pong]->unbindImageTexture(3, 0, GL_WRITE_ONLY, GL_R16UI);
      m_vertexNormalMap->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      m_vertexNormals.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_vertexPositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_projectedVertexPositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      vertexBufferCreationShader.useNoShader();

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<util::vec2i> projectedCachePos(m_options->specularCacheNumber);
      //m_projectedVertexPositions.getData(sizeof(util::vec2i) * 3580, m_options->specularCacheNumber * sizeof(util::vec2i) - sizeof(util::vec2i) * 3580, &projectedCachePos[0]);

      //std::vector<util::vec4f> cachePos(m_options->specularCacheNumber);
      //m_vertexPositions.getData(sizeof(util::vec4f) * 3580, m_options->specularCacheNumber * sizeof(util::vec4f) - sizeof(util::vec4f) * 3580, &cachePos[0]);

      //std::vector<util::vec4f> cacheNormalMat(m_options->specularCacheNumber);
      //m_vertexNormals.getData(sizeof(util::vec4f) * 3580, m_options->specularCacheNumber * sizeof(util::vec4f) - sizeof(util::vec4f) * 3580, &cacheNormalMat[0]);
    }

    void IndirectSpecularReflections::createSpecularCacheIndices()
    {
      util::vec4f clearValue2 = util::vec4f(0.0f);
      m_specularLightMap->clearTexture(&clearValue2[0]);

      {
        //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ////glMemoryBarrier(GL_ALL_BARRIER_BITS);
        //const sh::ComputeShader& specularCacheBoundingBoxComputationShader = m_shaderContainer->getComputeShader(m_specularCacheBoundingBoxComputation);
        //specularCacheBoundingBoxComputationShader.useShader();

        //m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        //m_cacheBBBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

        //m_cacheOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

        //sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        //m_cacheOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

        //m_cacheBBBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
        //m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        //specularCacheBoundingBoxComputationShader.useNoShader();


        //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        ////glMemoryBarrier(GL_ALL_BARRIER_BITS);

        //std::vector<util::vec4f> cacheBBs(2 * m_options->specularCacheNumber);
        //m_cacheBBBuffer.getData(0, 2 * m_options->specularCacheNumber * sizeof(util::vec4f), &cacheBBs[0]);

        //const sh::ComputeShader& specularCacheIndexListGenerationShader = m_shaderContainer->getComputeShader(m_specularCacheIndexListGeneration);
        //specularCacheIndexListGenerationShader.useShader();

        //m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        //m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
        //m_cacheBBBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

        //gBufferDepthMap->setTexture(0, 0);
        //gBufferNormalMap->setTexture(1, 1);

        //for(unsigned int i = 0; i < m_cacheIndexMap.size(); i++)
        //{
        //  m_cacheIndexMap[i]->bindImageTexture(i, 0, GL_WRITE_ONLY, GL_RGBA16UI);
        //}

        //m_cacheOffsetMap->bindImageTexture(2, 0, GL_READ_ONLY, GL_R32F);

        //sh::ComputeShader::dispatchComputeShader(128, 1, 1);

        //m_cacheOffsetMap->unbindImageTexture(2, 0, GL_READ_ONLY, GL_R32F);

        //for(unsigned int i = 0; i < m_cacheIndexMap.size(); i++)
        //{
        //  m_cacheIndexMap[i]->unbindImageTexture(i, 0, GL_WRITE_ONLY, GL_RGBA16UI);
        //}

        //gBufferNormalMap->unsetTexture(1);
        //gBufferDepthMap->unsetTexture(0);

        //m_cacheBBBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
        //m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
        //m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        //specularCacheIndexListGenerationShader.useNoShader();
      }

      //CPUTIMER("CacheIndexCPUTimer", 0)
      //GPUTIMER("CacheIndexOGLTimer", 1)

      util::vec4f clearVector(FLT_MAX);
      m_specularCacheIndexBuffer0.clearBuffer(GL_RGBA, GL_RGBA32F, GL_FLOAT, &clearVector[0]);
      m_specularCacheIndexBuffer1.clearBuffer(GL_RGBA, GL_RGBA32F, GL_FLOAT, &clearVector[0]);

      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      const sh::ComputeShader& specularCacheIndexListGenerationShader = m_shaderContainer->getComputeShader(m_specularCacheIndexListGeneration);
      specularCacheIndexListGenerationShader.useShader();

      sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &m_borderVertexNumber);

      m_cacheOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);
      m_cachePositionMap->bindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cacheNormalMap->bindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cacheAtomicIndexMap->bindImageTexture(3, 0, GL_READ_ONLY, GL_R32UI);

      m_vertexOffsetMap->bindImageTexture(4, 0, GL_READ_ONLY, GL_R32F);
      m_vertexPositionMap->bindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexNormalMap->bindImageTexture(6, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexAtomicIndexMap->bindImageTexture(7, 0, GL_READ_ONLY, GL_R32UI);

      m_vertexPositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_vertexNormals.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_specularCacheIndexBuffer0.bindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_specularCacheIndexBuffer1.bindBuffer(GL_SHADER_STORAGE_BUFFER, 5);

      sh::ComputeShader::dispatchComputeShader(4096, 1, 1);

      m_specularCacheIndexBuffer1.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 5);
      m_specularCacheIndexBuffer0.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_vertexNormals.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_vertexPositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_vertexAtomicIndexMap->unbindImageTexture(7, 0, GL_READ_ONLY, GL_R32UI);
      m_vertexNormalMap->unbindImageTexture(6, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexPositionMap->unbindImageTexture(5, 0, GL_READ_ONLY, GL_RGBA32F);
      m_vertexOffsetMap->unbindImageTexture(4, 0, GL_READ_ONLY, GL_R32F);

      m_cacheAtomicIndexMap->unbindImageTexture(3, 0, GL_READ_ONLY, GL_R32UI);
      m_cacheNormalMap->unbindImageTexture(2, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cachePositionMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_RGBA32F);
      m_cacheOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      specularCacheIndexListGenerationShader.useNoShader();

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      //std::vector<util::vec4f> cacheIndices0(m_options->specularCacheNumber);
      //m_specularCacheIndexBuffer0.getData(sizeof(util::vec4f) * 3580, m_specularCacheIndexBuffer0.getBufferSize() - sizeof(util::vec4f) * 3580, &cacheIndices0[0]);

      //std::vector<util::vec4f> cacheIndices1(m_options->specularCacheNumber);
      //m_specularCacheIndexBuffer1.getData(sizeof(util::vec4f) * 3580, m_specularCacheIndexBuffer1.getBufferSize() - sizeof(util::vec4f) * 3580, &cacheIndices1[0]);
    }

    void IndirectSpecularReflections::createVoronoiDiagram(const GPUBuffer& projectedPositions)
    {
      //CPUTIMER("VoronoiDiagramCreationCPUTimer", 0)
      //GPUTIMER("VoronoiDiagramCreationOGLTimer", 1)

      {
        unsigned int stepNumber = log(float(m_options->width)) / log(2.0f);

        const sh::RenderShader& voronoiShader = m_shaderContainer->getRenderShader(m_specularJumpFloodAlgorithm);

        voronoiShader.useShader();

        projectedPositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        for(int i = stepNumber - 1; i >= 0; i--)
        {
          m_voronoiRenderQuad.setRenderTargets(1, m_voronoiDiagram[m_ping]);

          m_voronoiRenderQuad.setWriteFrameBuffer();

          m_voronoiDiagram[m_pong]->setTexture(1, 1);

          int stepSize = powf(2.0f, i);
          sh::RenderShader::setUniform(0, GL_INT, &stepSize);

          m_voronoiRenderQuad.render();

          if(i > 0)
          {
            m_ping = m_pong;
            m_pong = 1 - m_ping;
          }
        }

        m_voronoiDiagram[m_pong]->unsetTexture(1);

        projectedPositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_voronoiRenderQuad.unsetWriteFrameBuffer();

        voronoiShader.useNoShader();
      }
      //std::vector<unsigned int> indexData0(m_options->width * m_options->height);
      //m_voronoiDiagram[m_pong]->getTextureData(&indexData0[0]);

      //std::vector<unsigned int> indexData1(m_options->width * m_options->height);
      //m_voronoiDiagram[m_ping]->getTextureData(&indexData1[0]);
      {
        const sh::RenderShader& voronoiIslandRemoveShader = m_shaderContainer->getRenderShader(m_specularJumpFloodIslandRemovalAlgorithm);

        voronoiIslandRemoveShader.useShader();

        projectedPositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        for(unsigned int i = 0; i < 3; i++)
        {
          m_voronoiRenderQuad.setRenderTargets(1, m_voronoiDiagram[m_pong]);

          m_voronoiRenderQuad.setWriteFrameBuffer();

          m_voronoiDiagram[m_ping]->bindImageTexture(0, 0, GL_READ_ONLY, GL_R16UI);

          m_voronoiRenderQuad.render();

          m_voronoiDiagram[m_ping]->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R16UI);

          m_ping = m_pong;
          m_pong = 1 - m_ping;

          glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        projectedPositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_voronoiRenderQuad.unsetWriteFrameBuffer();

        voronoiIslandRemoveShader.useNoShader();
      }

      {
        const sh::ComputeShader& voronoiCornerRepairShader = m_shaderContainer->getComputeShader(m_specularCacheVoronoiDiagramCornerRepair);
        voronoiCornerRepairShader.useShader();

        projectedPositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_voronoiDiagram[m_ping]->bindImageTexture(0, 0, GL_READ_WRITE, GL_R16UI);

        sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &m_borderVertexNumber);

        sh::ComputeShader::dispatchComputeShader(4, 1, 1);

        m_voronoiDiagram[m_ping]->unbindImageTexture(0, 0, GL_READ_WRITE, GL_R16UI);

        projectedPositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        voronoiCornerRepairShader.useNoShader();
      }

      //std::vector<unsigned int> indexData1(m_options->width * m_options->height);
      //m_voronoiDiagram[m_ping]->getTextureData(&indexData1[0]);
    }

    void IndirectSpecularReflections::calculateTriangleIndexOffsets()
    {
      //CPUTIMER("TriangleIndexOffsetCPUTimer", 0)
      //GPUTIMER("TriangleIndexOffsetOGLTimer", 1)

      float clearValue = 0.0;
      m_triangleIndexOffsets->clearTexture(&clearValue);

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      const sh::ComputeShader& specularCacheTriangleIndexOffsetCalculationShader = m_shaderContainer->getComputeShader(m_specularCacheTriangleIndexOffsetCalculation);
      specularCacheTriangleIndexOffsetCalculationShader.useShader();

      m_voronoiDiagram[m_ping]->bindImageTexture(0, 0, GL_READ_ONLY, GL_R16UI);
      m_triangleIndexOffsets->bindImageTexture(1, 0, GL_WRITE_ONLY, GL_R32F);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_triangleIndexOffsets->unbindImageTexture(1, 0, GL_WRITE_ONLY, GL_R32F);
      m_voronoiDiagram[m_ping]->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R16UI);

      specularCacheTriangleIndexOffsetCalculationShader.useNoShader();

      createOffsets(m_triangleIndexOffsets);
    }

    void IndirectSpecularReflections::fillTriangleIndexBuffer()
    {
      //CPUTIMER("FillTriangleIndicesCPUTimer", 0)
      //GPUTIMER("FillTriangleIndicesOGLTimer", 1)

      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      unsigned int clearVal = 0;
      m_triangleIndexBuffer.clearBuffer(GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT, &clearVal);//eliminates the problem of bad triangles appearing somewhere, but it shouldnt be neccessary because every index which is being used will be overwritten anyway, except there is a bug in the offset or index calculation

      const sh::ComputeShader& specularCacheFillTriangleIndices = m_shaderContainer->getComputeShader(m_specularCacheFillTriangleIndices);
      specularCacheFillTriangleIndices.useShader();

      m_voronoiDiagram[m_ping]->bindImageTexture(0, 0, GL_READ_ONLY, GL_R16UI);
      m_triangleIndexOffsets->bindImageTexture(1, 0, GL_READ_ONLY, GL_R32F);

      m_triangleIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_triangleCommandBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_triangleCommandBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_triangleIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_triangleIndexOffsets->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R32F);
      m_voronoiDiagram[m_ping]->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R16UI);

      specularCacheFillTriangleIndices.useNoShader();

      //std::vector<unsigned int> offsets(3 * (2 * m_options->specularCacheNumber - 4));
      //m_triangleIndexBuffer.getData(0, m_triangleIndexBuffer.getBufferSize(), &offsets[0]);

      //DrawElementsIndirectCommand command;
      //m_triangleCommandBuffer.getData(0, sizeof(DrawElementsIndirectCommand), &command);
    }

    void IndirectSpecularReflections::createTriangleSpecularProxyLightMap(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap,
      util::SharedPointer<db::Texture2D> gBufferLinearDepthMap)
    {
      //CPUTIMER("SpecularProxyLightMapCPUTimer", 0)
      //GPUTIMER("SpecularProxyLightMapOGLTimer", 1)

      util::vec4f clearValue = util::vec4f(0.0f);
      m_specularLightMap->clearTexture(&clearValue[0]);

      util::vec4ui clearVectorValueUI = util::vec4ui(UINT32_MAX);
      for(unsigned int i = 0; i < 6; i++)
      {
        m_cacheIndices[i]->clearTexture(&clearVectorValueUI[0]);
      }

      //glMemoryBarrier(GL_ALL_BARRIER_BITS);
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      m_specularProxyLightQuad.setWriteFrameBuffer();

      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);
      glDepthFunc(GL_ALWAYS);

      const sh::RenderShader& delaunayShader = m_shaderContainer->getRenderShader(m_specularLightMapDelaunayTriangulationCreation);

      delaunayShader.useShader();

      glBindVertexArray(m_triangleVAO);
      glEnableVertexAttribArray(sh::RenderShader::POSITION);
      glEnableVertexAttribArray(sh::RenderShader::NORMAL);
      glEnableVertexAttribArray(sh::RenderShader::CACHEINDICES0);
      glEnableVertexAttribArray(sh::RenderShader::CACHEINDICES1);

      m_vertexPositions.bindVertexbuffer(0, 0, sizeof(util::vec4f));
      m_vertexNormals.bindVertexbuffer(1, 0, sizeof(util::vec4f));
      m_specularCacheIndexBuffer0.bindVertexbuffer(2, 0, sizeof(util::vec4f));
      m_specularCacheIndexBuffer1.bindVertexbuffer(3, 0, sizeof(util::vec4f));

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);
      gBufferLinearDepthMap->setTexture(3, 3);

      for(unsigned int i = 0; i < 6; i++)
      {
        m_cacheIndices[i]->bindImageTexture(i, 0, GL_WRITE_ONLY, GL_RGBA16UI);
      }

      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_proxyLightPositionBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_proxyLightLuminousFluxBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

      m_triangleCommandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);

      m_triangleIndexBuffer.bindBuffer(GL_ELEMENT_ARRAY_BUFFER);

      glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);

      m_triangleIndexBuffer.unbindBuffer(GL_ELEMENT_ARRAY_BUFFER);

      m_triangleCommandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);

      m_proxyLightLuminousFluxBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_proxyLightPositionBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      for(unsigned int i = 0; i < 6; i++)
      {
        m_cacheIndices[i]->unbindImageTexture(i, 0, GL_WRITE_ONLY, GL_RGBA16UI);
      }

      gBufferLinearDepthMap->unsetTexture(3);
      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      m_specularCacheIndexBuffer1.unbindVertexBuffer(3);
      m_specularCacheIndexBuffer0.unbindVertexBuffer(2);
      m_vertexNormals.unbindVertexBuffer(1);
      m_vertexPositions.unbindVertexBuffer(0);

      glDisableVertexAttribArray(sh::RenderShader::CACHEINDICES1);
      glDisableVertexAttribArray(sh::RenderShader::CACHEINDICES0);
      glDisableVertexAttribArray(sh::RenderShader::NORMAL);
      glDisableVertexAttribArray(sh::RenderShader::POSITION);
      glBindVertexArray(0);

      delaunayShader.useNoShader();

      glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);
      glEnable(GL_DEPTH_TEST);

      m_specularProxyLightQuad.unsetWriteFrameBuffer();
    }

    void IndirectSpecularReflections::specularErrorSpotLight(
      util::SharedPointer<db::Texture2D> gBufferDepthMap,
      util::SharedPointer<db::Texture2D> gBufferNormalMap,
      util::SharedPointer<db::Texture2D> gBufferMaterialMap)
    {
      //CPUTIMER("SpecularMapErrorSpotCPUTimer", 0)
      //GPUTIMER("SpecularMapErrorSpotOGLTimer", 1)

      glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

      m_specularProxyLightQuad.setWriteFrameBuffer();

      const sh::RenderShader& errorSpotLightShader = m_shaderContainer->getRenderShader(m_indirectSpecularErrorSpotLighting);
      errorSpotLightShader.useShader();

      gBufferDepthMap->setTexture(0, 0);
      gBufferNormalMap->setTexture(1, 1);
      gBufferMaterialMap->setTexture(2, 2);

      for(unsigned int i = 0; i < 6; i++)
      {
        m_cacheIndices[i]->setTexture(3 + i, 3 + i);
        //m_cacheIndices[i]->bindImageTexture(i, 0, GL_READ_ONLY, GL_RGBA16UI);
      }

      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cacheNormalMaterial.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_proxyLightPositionBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_proxyLightLuminousFluxBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

      m_specularProxyLightQuad.render();

      m_proxyLightLuminousFluxBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_proxyLightPositionBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cacheNormalMaterial.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      for(unsigned int i = 0; i < 6; i++)
      {
        //m_cacheIndices[i]->unbindImageTexture(i, 0, GL_READ_ONLY, GL_RGBA16UI);
        m_cacheIndices[i]->unsetTexture(3 + i);
      }

      gBufferMaterialMap->unsetTexture(2);
      gBufferNormalMap->unsetTexture(1);
      gBufferDepthMap->unsetTexture(0);

      errorSpotLightShader.useNoShader();

      m_specularProxyLightQuad.unsetWriteFrameBuffer();
    }

    void IndirectSpecularReflections::createCacheVertexDebugLines()
    {
      util::vec4f clearVal = util::vec4f(0.0f);
      m_debugCacheVertexIndexPositions.clearBuffer(GL_RGBA, GL_RGBA32F, GL_FLOAT, &clearVal[0]);

      const sh::ComputeShader& specularDebugCacheVertexIndexBufferCreation = m_shaderContainer->getComputeShader(m_specularDebugCacheVertexIndexBufferCreation);
      specularDebugCacheVertexIndexBufferCreation.useShader();

      sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &m_borderVertexNumber);

      m_vertexOffsetMap->bindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);
      m_cacheOffsetMap->bindImageTexture(1, 0, GL_READ_ONLY, GL_R32F);

      m_vertexPositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
      m_cachePositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_specularCacheIndexBuffer0.bindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_specularCacheIndexBuffer1.bindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_debugCacheVertexIndexPositions.bindBuffer(GL_SHADER_STORAGE_BUFFER, 4);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_debugCacheVertexIndexPositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 4);
      m_specularCacheIndexBuffer1.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 3);
      m_specularCacheIndexBuffer0.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 2);
      m_cachePositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_vertexPositions.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_cacheOffsetMap->unbindImageTexture(1, 0, GL_READ_ONLY, GL_R32F);
      m_vertexOffsetMap->unbindImageTexture(0, 0, GL_READ_ONLY, GL_R32F);

      specularDebugCacheVertexIndexBufferCreation.useNoShader();
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getDebugCachePositionsMapFilterInnerX() const
    {
      return m_cachePositionBufferInnerX;
    }

    util::SharedPointer<db::Texture2D> IndirectSpecularReflections::getSpecularLightMap() const
    {
      return m_specularLightMap; 
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

    void IndirectSpecularReflections::showVoronoiDiagram()
    {
      std::vector<float> offsets(m_options->width * m_options->height);
      m_vertexOffsetMap->getTextureData(&offsets[0]);

      unsigned int vertexNumber = offsets[m_options->width * m_options->height - 1] + m_borderVertexNumber;

      if(vertexNumber > 0)
      {
        std::vector<util::vec3ub> siteColors(vertexNumber);
        for(unsigned int i = 0; i < siteColors.size(); i++)
        {
          siteColors[i] = util::vec3ub(255 * util::RandomSequenceGenerator::halton(i + 1, 2), 255 * util::RandomSequenceGenerator::halton(i + 1, 3), 255 * util::RandomSequenceGenerator::halton(i + 1, 5));
        }

        m_voronoiColorTexture = util::SharedPointer<db::Texture1D>(new db::Texture1D(siteColors.size(), GL_TEXTURE_1D, GL_UNSIGNED_BYTE, GL_RGB8, GL_RGB, 3, 24, &siteColors[0]));
      }

      const sh::RenderShader& showVoronoiShader = m_shaderContainer->getRenderShader(m_showVoronoiDiagram);

      showVoronoiShader.useShader();

      sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &vertexNumber);

      m_voronoiDiagram[m_ping]->setTexture(0, 0);
      m_voronoiColorTexture->setTexture(1, 1);

      m_voronoiRenderQuad.render();

      m_voronoiColorTexture->unsetTexture(1);
      m_voronoiDiagram[m_ping]->unsetTexture(0);

      showVoronoiShader.useNoShader();
    }

    void IndirectSpecularReflections::showIncompleteVoronoiDiagram()
    {
      std::vector<float> offsets(m_options->width * m_options->height);
      m_vertexOffsetMap->getTextureData(&offsets[0]);

      unsigned int vertexNumber = offsets[m_options->width * m_options->height - 1] + m_borderVertexNumber;

      if(vertexNumber > 0)
      {
        std::vector<util::vec3ub> siteColors(vertexNumber);
        for(unsigned int i = 0; i < siteColors.size(); i++)
        {
          siteColors[i] = util::vec3ub(255 * util::RandomSequenceGenerator::halton(i + 1, 2), 255 * util::RandomSequenceGenerator::halton(i + 1, 3), 255 * util::RandomSequenceGenerator::halton(i + 1, 5));
        }

        m_voronoiColorTexture = util::SharedPointer<db::Texture1D>(new db::Texture1D(siteColors.size(), GL_TEXTURE_1D, GL_UNSIGNED_BYTE, GL_RGB8, GL_RGB, 3, 24, &siteColors[0]));
      }

      const sh::RenderShader& showVoronoiShader = m_shaderContainer->getRenderShader(m_showVoronoiDiagram);

      showVoronoiShader.useShader();

      sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &vertexNumber);

      m_voronoiDiagram[m_pong]->setTexture(0, 0);
      m_voronoiColorTexture->setTexture(1, 1);

      m_voronoiRenderQuad.render();

      m_voronoiColorTexture->unsetTexture(1);
      m_voronoiDiagram[m_pong]->unsetTexture(0);

      showVoronoiShader.useNoShader();
    }

    void IndirectSpecularReflections::showDelaunayTriangulation()
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);
      glDepthFunc(GL_ALWAYS);
      glClear(GL_COLOR_BUFFER_BIT);

      const sh::RenderShader& delaunayShader = m_shaderContainer->getRenderShader(m_showSpecularCacheDelaunayTriangulation);

      delaunayShader.useShader();

      glBindVertexArray(m_simpleTriangleVAO);
      glEnableVertexAttribArray(sh::RenderShader::POSITION);

      m_vertexPositions.bindVertexbuffer(0, 0, sizeof(util::vec4f));

      m_triangleCommandBuffer.bindBuffer(GL_DRAW_INDIRECT_BUFFER);

      m_triangleIndexBuffer.bindBuffer(GL_ELEMENT_ARRAY_BUFFER);

      glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);

      m_triangleIndexBuffer.unbindBuffer(GL_ELEMENT_ARRAY_BUFFER);

      m_triangleCommandBuffer.unbindBuffer(GL_DRAW_INDIRECT_BUFFER);

      m_vertexPositions.unbindVertexBuffer(0);

      glDisableVertexAttribArray(sh::RenderShader::POSITION);
      glBindVertexArray(0);

      delaunayShader.useNoShader();

      glDepthFunc(GL_LESS);
      glDepthMask(GL_TRUE);
      glEnable(GL_DEPTH_TEST);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void IndirectSpecularReflections::showVertexCacheIndexLines()
    {
      createCacheVertexDebugLines();

      std::vector<float> offsets(m_options->width * m_options->height);
      m_vertexOffsetMap->getTextureData(&offsets[0]);

      unsigned int vertexNumber = 2 * 8 * offsets[m_options->width * m_options->height - 1];

      glClear(GL_COLOR_BUFFER_BIT);

      const sh::RenderShader& showLines = m_shaderContainer->getRenderShader(m_showLines);

      showLines.useShader();

      glBindVertexArray(m_simpleTriangleVAO);
      glEnableVertexAttribArray(sh::RenderShader::POSITION);

      m_debugCacheVertexIndexPositions.bindVertexbuffer(0, 0, sizeof(util::vec4f));

      glDrawArrays(GL_LINES, 0, vertexNumber);

      m_debugCacheVertexIndexPositions.unbindVertexBuffer(0);

      glDisableVertexAttribArray(sh::RenderShader::POSITION);
      glBindVertexArray(0);

      showLines.useNoShader();
    }
  }
}
