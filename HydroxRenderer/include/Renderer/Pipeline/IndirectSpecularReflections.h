#ifndef INDIRECTSPECULARREFLECTIONS_H_
#define INDIRECTSPECULARREFLECTIONS_H_

#include <list>

#include <GL/glew.h>

#include <Utilities/Timer/MeasurementManager.h>

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

      void updateBuffer(unsigned int reflectiveShadowMapNumber, unsigned int globalOccluderNumber);

      void generateReflectionCaches(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferLinearDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferVertexNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        util::SharedPointer<db::Texture3D> indirectLightNormals,
        util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
        util::SharedPointer<db::Texture3D> indirectShadowMaps,
        util::SharedPointer<db::Texture2D> indirectShadowVALQuaternions,
        const GPUBuffer& occluderBuffer);

      util::SharedPointer<db::Texture2D> getDebugCachePositionsMapFilterInnerX() const;

      util::SharedPointer<db::Texture2D> getDownsampledSpecularLightMap() const;
      util::SharedPointer<db::Texture2D> getSpecularLightMap() const;

      util::SharedPointer<db::Texture2D> getDebugCachePositions() const;
      util::SharedPointer<db::Texture2D> getDebugCacheNormals() const;

      util::SharedPointer<db::Texture2D> getDebugVertexPositions() const;
      util::SharedPointer<db::Texture2D> getDebugVertexNormals() const;

      util::SharedPointer<db::Texture2D> getDebugVertexBlockNumber() const;
      util::SharedPointer<db::Texture2D> getDebugCacheBlockNumber() const;

      util::SharedPointer<db::Texture2D> getDebugGBufferHalfResDepthMap() const;
      util::SharedPointer<db::Texture2D> getDebugGBufferHalfResLinearDepthMap() const;
      util::SharedPointer<db::Texture2D> getDebugGBufferHalfResNormalMap() const;
      util::SharedPointer<db::Texture2D> getDebugGBufferHalfResVertexNormalMap() const;
      util::SharedPointer<db::Texture2D> getDebugGBufferHalfResMaterialMap() const;

      void gBufferDownsampling(util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferLinearDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferVertexNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap);

    private:
      
      IndirectSpecularReflections(const IndirectSpecularReflections&);
      IndirectSpecularReflections& operator=(const IndirectSpecularReflections&);

      void createTubeData(util::SharedPointer<db::Texture3D> indirectLightPositions);

      void createSpecularInnerCaches(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                                     util::SharedPointer<db::Texture2D> gBufferVertexNormalMap,
                                     util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      void createSpecularEdgeCaches(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferVertexNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
        util::SharedPointer<db::Texture2D> gBufferLinearDepthMap);

      void createEdgeVertices(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferVertexNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
        util::SharedPointer<db::Texture2D> gBufferLinearDepthMap);

      void createOffsets(util::SharedPointer<db::Texture2D> offsetMap);

      void createSpecularCacheBuffer();

      void createProxyLightBuffer(
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        util::SharedPointer<db::Texture3D> indirectLightNormals,
        util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
        util::SharedPointer<db::Texture3D> indirectShadowMaps,
        util::SharedPointer<db::Texture2D> indirectShadowVALQuaternions,
        const GPUBuffer& occluderBuffer);

      void createSpecularCacheIndices();

      void createIndirectLightMap(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      void upsampleIndirectLightMap(
        util::SharedPointer<db::Texture2D> fullResGBufferLinearDepthMap,
        util::SharedPointer<db::Texture2D> fullResGBufferNormalMap);

      sh::ComputeShaderHandle m_gBufferDownsampling;
      sh::ComputeShaderHandle m_calculateAreaLightTube;
      sh::ComputeShaderHandle m_specularCachePositionFilterX;
      sh::ComputeShaderHandle m_specularCachePositionFilterY;
      sh::ComputeShaderHandle m_specularCacheOffsetUpSweep;
      sh::ComputeShaderHandle m_specularCacheOffsetDownSweep;
      sh::ComputeShaderHandle m_specularCacheBufferCreation;
      sh::ComputeShaderHandle m_specularProxyLightBufferCreation;
      sh::ComputeShaderHandle m_specularCacheIndexListGeneration;
      sh::ComputeShaderHandle m_specularCacheEdgeCacheCreation;
      sh::ComputeShaderHandle m_specularCacheEdgeVertexCreation;
      sh::ComputeShaderHandle m_indirectLightMapCreation;
      sh::ComputeShaderHandle m_indirectLightMapUpsampling;

      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      util::SharedPointer<util::MeasurementManager> m_measurementManager;

      GPUBuffer m_proxyLightTubeBuffer;

      GPUBuffer m_cacheIndexBuffer;

      GPUBuffer m_cachePositions;
      GPUBuffer m_cacheNormalMaterial;
      GPUBuffer m_proxyLightPositionBuffer;
      GPUBuffer m_proxyLightLuminousFluxBuffer;

      GPUBuffer m_debugCacheVertexIndexPositions;

      util::SharedPointer<db::Texture2D> m_halfResGBufferDepthMap0;
      util::SharedPointer<db::Texture2D> m_halfResGBufferLinearDepthMap0;
      util::SharedPointer<db::Texture2D> m_halfResGBufferNormalMap0;
      util::SharedPointer<db::Texture2D> m_halfResGBufferVertexNormalMap0;
      util::SharedPointer<db::Texture2D> m_halfResGBufferMaterialMap0;

      util::SharedPointer<db::Texture2D> m_halfResGBufferDepthMap1;
      util::SharedPointer<db::Texture2D> m_halfResGBufferLinearDepthMap1;
      util::SharedPointer<db::Texture2D> m_halfResGBufferNormalMap1;
      util::SharedPointer<db::Texture2D> m_halfResGBufferVertexNormalMap1;
      util::SharedPointer<db::Texture2D> m_halfResGBufferMaterialMap1;

      util::SharedPointer<db::Texture2D> m_vertexAtomicIndexMap;
      util::SharedPointer<db::Texture2D> m_vertexPositionMap;
      util::SharedPointer<db::Texture2D> m_vertexNormalMap;
 
      util::SharedPointer<db::Texture2D> m_cacheAtomicIndexMap;
      util::SharedPointer<db::Texture2D> m_cachePositionMap;
      util::SharedPointer<db::Texture2D> m_cacheNormalMap;
      util::SharedPointer<db::Texture2D> m_cacheOffsetMap;

      util::SharedPointer<db::Texture2D> m_cachePositionBufferInnerX;

      util::SharedPointer<db::Texture2D> m_downsampledSpecularLightMap;
      util::SharedPointer<db::Texture2D> m_specularLightMap;

      util::vec2i m_indirectLightResolution;

      unsigned int m_ping;
      unsigned int m_pong;

      unsigned int m_vertexIndexMultiplicator;
      unsigned int m_vertexResolution;
      unsigned int m_cacheResolution;
      unsigned int m_globalOccluderNumber;

      unsigned int m_reflectiveShadowMapNumber;
      unsigned int m_debugCacheNumber;
    };
  }
}

#endif
