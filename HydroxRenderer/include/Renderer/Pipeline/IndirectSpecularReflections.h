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

      void updateBuffer(unsigned int cacheNumber);

      void generateReflectionCaches(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        util::SharedPointer<db::Texture3D> indirectLightNormals,
        util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
        const GPUBuffer& samplingPatternBuffer);

      util::SharedPointer<db::Texture2D> getDebugCachePositionsMap() const;
      util::SharedPointer<db::Texture2D> getDebugIndexCounterMap() const;
      util::SharedPointer<db::Texture2D> getSpecularLightMap() const;

    private:
      
      IndirectSpecularReflections(const IndirectSpecularReflections&);
      IndirectSpecularReflections& operator=(const IndirectSpecularReflections&);

      void createTubeData(util::SharedPointer<db::Texture3D> indirectLightPositions, const GPUBuffer& samplingPatternBuffer);

      void createSpecularCachePositions(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                                        util::SharedPointer<db::Texture2D> gBufferNormalMap,
                                        util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      void createProxyLightOffsets();

      void createSpecularCaches(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      void createProxyLights(
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        util::SharedPointer<db::Texture3D> indirectLightNormals,
        util::SharedPointer<db::Texture3D> indirectLightLuminousFlux,
        const GPUBuffer& samplingPatternBuffer);

      void createSpecularCacheIndexMap(util::SharedPointer<db::Texture2D> gBufferDepthMap, util::SharedPointer<db::Texture2D> gBufferNormalMap);

      void createSpecularProxyLightMap(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture2D> gBufferMaterialMap);

      sh::ComputeShaderHandle m_calculateAreaLightTube;
      sh::ComputeShaderHandle m_specularCacheXCreation;
      sh::ComputeShaderHandle m_specularCacheYCreation;
      sh::ComputeShaderHandle m_specularCacheOffsetUpSweep;
      sh::ComputeShaderHandle m_specularCacheOffsetDownSweep;
      sh::ComputeShaderHandle m_specularCacheCreation;
      sh::ComputeShaderHandle m_specularProxyLightCreation;
      sh::RenderShaderHandle m_specularCacheIndexMapCreation;
      sh::RenderShaderHandle m_specularLightMapCreation;
 
      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      GLuint m_pointVAO;
      GPUBuffer m_proxyLightTubeBuffer;

      GPUBuffer m_commandBuffer;
      GPUBuffer m_cachePositions;
      GPUBuffer m_cacheNormalMaterial;
      GPUBuffer m_proxyLightPositionBuffer;
      GPUBuffer m_proxyLightLuminousFluxBuffer;
      
      util::SharedPointer<db::Texture2D> m_cachePositionBuffer0;
      util::SharedPointer<db::Texture2D> m_cachePositionBuffer1;
      util::SharedPointer<db::Texture2D> m_cacheOffsets;
      util::SharedPointer<db::Texture2D> m_cacheIndexAtomicCounter;
      std::vector<util::SharedPointer<db::Texture2D>> m_cacheIndexMap;
      util::SharedPointer<db::Texture2D> m_specularLightMap;
      Renderquad m_specularIndexQuad;
      Renderquad m_specularProxyLightQuad;

      unsigned int m_reflectiveShadowMapNumber;
    };
  }
}

#endif
