#ifndef INDIRECTSPECULARREFLECTIONS_H_
#define INDIRECTSPECULARREFLECTIONS_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/Texture1D.h>
#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

#include <Shader/ShaderContainer.h>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Buffer/UBO.h"
#include "Renderer/Buffer/TBO.h"
#include "Renderer/Pipeline/Renderquad.h"
#include "Renderer/Pipeline/CameraManager.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class RenderOptions;

    class IndirectSpecularReflections
    {
    public:

      IndirectSpecularReflections();
      ~IndirectSpecularReflections();

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer(unsigned int reflectiveShadowMapNumber);

      void generateReflectionCaches(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferLinearDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        util::SharedPointer<db::Texture3D> indirectLightNormals,
        util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
        const GPUBuffer& samplingPatternBuffer);

      util::SharedPointer<db::Texture2D> getDebugCachePositionsMapFilterInnerX() const;

      util::SharedPointer<db::Texture2D> getSpecularLightMap() const;

      util::SharedPointer<db::Texture2D> getDebugEdgeCachePositions() const;
      util::SharedPointer<db::Texture2D> getDebugEdgeCacheNormals() const;

      util::SharedPointer<db::Texture2D> getDebugVertexBlockNumber() const;
      util::SharedPointer<db::Texture2D> getDebugCacheBlockNumber() const;

      void showVoronoiDiagram();

      void showIncompleteVoronoiDiagram();

      void showDelaunayTriangulation();

      void showVertexCacheIndexLines();

    private:
      
      IndirectSpecularReflections(const IndirectSpecularReflections&);
      IndirectSpecularReflections& operator=(const IndirectSpecularReflections&);

      void createTubeData(util::SharedPointer<db::Texture3D> indirectLightPositions, const GPUBuffer& samplingPatternBuffer);

      void createSpecularInnerCaches(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                                        util::SharedPointer<db::Texture2D> gBufferNormalMap,
                                        util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      void createSpecularEdgeCaches(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
        util::SharedPointer<db::Texture2D> gBufferLinearDepthMap);

      void createEdgeVertices(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
        util::SharedPointer<db::Texture2D> gBufferLinearDepthMap);

      void createOffsets(util::SharedPointer<db::Texture2D> offsetMap);

      void createSpecularCacheBuffer(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      void createProxyLightBuffer(
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        util::SharedPointer<db::Texture3D> indirectLightNormals,
        util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
        const GPUBuffer& samplingPatternBuffer);

      void createVertexBuffer(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap);

      void createSpecularCacheIndices(util::SharedPointer<db::Texture2D> gBufferDepthMap, util::SharedPointer<db::Texture2D> gBufferNormalMap);

      void createVoronoiDiagram(const GPUBuffer& projectedPositions);

      void calculateTriangleIndexOffsets();

      void fillTriangleIndexBuffer();

      void createTriangleSpecularProxyLightMap(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
        util::SharedPointer<db::Texture2D> gBufferLinearDepthMap);

      void specularErrorSpotLight(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      void createCacheVertexDebugLines();

      sh::ComputeShaderHandle m_calculateAreaLightTube;
      sh::ComputeShaderHandle m_specularCachePositionFilterX;
      sh::ComputeShaderHandle m_specularCachePositionFilterY;
      sh::ComputeShaderHandle m_reduceSpecularCachePositionShader;
      sh::ComputeShaderHandle m_specularCacheOffsetUpSweep;
      sh::ComputeShaderHandle m_specularCacheOffsetDownSweep;
      sh::ComputeShaderHandle m_specularCacheBufferCreation;
      sh::ComputeShaderHandle m_specularProxyLightBufferCreation;
      sh::ComputeShaderHandle m_specularCacheVertexBufferCreation;
      sh::ComputeShaderHandle m_specularCacheBoundingBoxComputation;
      sh::ComputeShaderHandle m_specularCacheIndexListGeneration;
      sh::ComputeShaderHandle m_specularCacheTriangleIndexOffsetCalculation;
      sh::ComputeShaderHandle m_specularCacheFillTriangleIndices;
      sh::ComputeShaderHandle m_specularCacheEdgeCacheCreation;
      sh::ComputeShaderHandle m_specularCacheVoronoiDiagramCornerRepair;
      sh::ComputeShaderHandle m_specularCacheEdgeVertexCreation;
      sh::ComputeShaderHandle m_specularDebugCacheVertexIndexBufferCreation;

      sh::RenderShaderHandle m_specularJumpFloodAlgorithm;
      sh::RenderShaderHandle m_specularJumpFloodIslandRemovalAlgorithm;
      sh::RenderShaderHandle m_showVoronoiDiagram;
      sh::RenderShaderHandle m_showSpecularCacheDelaunayTriangulation;
      sh::RenderShaderHandle m_specularLightMapDelaunayTriangulationCreation;
      sh::RenderShaderHandle m_indirectSpecularErrorSpotLighting;
      sh::RenderShaderHandle m_showLines;
 
      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      GLuint m_simpleTriangleVAO;
      GLuint m_triangleVAO;
      GPUBuffer m_proxyLightTubeBuffer;

      GPUBuffer m_triangleCommandBuffer;
      GPUBuffer m_triangleIndexBuffer;

      GPUBuffer m_projectedVertexPositions;
      GPUBuffer m_vertexPositions;
      GPUBuffer m_vertexNormals;

      GPUBuffer m_specularCacheIndexBuffer0;
      GPUBuffer m_specularCacheIndexBuffer1;

      GPUBuffer m_cacheBBBuffer;

      GPUBuffer m_cachePositions;
      GPUBuffer m_cacheNormalMaterial;
      GPUBuffer m_proxyLightPositionBuffer;
      GPUBuffer m_proxyLightLuminousFluxBuffer;

      GPUBuffer m_debugCacheVertexIndexPositions;

      util::SharedPointer<db::Texture2D> m_vertexAtomicIndexMap;
      util::SharedPointer<db::Texture2D> m_vertexPositionMap;
      util::SharedPointer<db::Texture2D> m_vertexNormalMap;
      util::SharedPointer<db::Texture2D> m_vertexOffsetMap;

      util::SharedPointer<db::Texture2D> m_cacheAtomicIndexMap;
      util::SharedPointer<db::Texture2D> m_edgeCachePositionMap;
      util::SharedPointer<db::Texture2D> m_edgeCacheNormalMap;
      util::SharedPointer<db::Texture2D> m_edgeCacheOffsetMap;

      util::SharedPointer<db::Texture2D> m_cachePositionBufferInnerX;

      util::SharedPointer<db::Texture2D> m_triangleIndexOffsets;

      std::vector<util::SharedPointer<db::Texture2D>> m_voronoiDiagram;
      util::SharedPointer<db::Texture1D> m_voronoiColorTexture;

      std::vector<util::SharedPointer<db::Texture2D>> m_cacheIndices;

      util::SharedPointer<db::Texture2D> m_specularLightMap;

      Renderquad m_voronoiRenderQuad;
      Renderquad m_specularIndexQuad;
      Renderquad m_specularProxyLightQuad;

      unsigned int m_ping;
      unsigned int m_pong;

      unsigned int m_vertexResolution;
      unsigned int m_cacheResolution;

      unsigned int m_reflectiveShadowMapNumber;
      unsigned int m_debugCacheNumber;
      unsigned int m_borderVertexNumber;
    };
  }
}

#endif
