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
        const GPUBuffer& samplingPatternBuffer);

      util::SharedPointer<db::Texture2D> getSpecularLightMap() const;

    private:
      
      IndirectSpecularReflections(const IndirectSpecularReflections&);
      IndirectSpecularReflections& operator=(const IndirectSpecularReflections&);

      void createTubeData(util::SharedPointer<db::Texture3D> indirectLightPositions);

      void createSpecularCachePositions(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                                        util::SharedPointer<db::Texture2D> gBufferNormalMap,
                                        util::SharedPointer<db::Texture2D> gBufferMaterialMap,
                                        util::SharedPointer<db::Texture3D> indirectLightPositions,
                                        const GPUBuffer& samplingPatternBuffer);

      void createProxyLightOffsets();

      void createProxyLights(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                             util::SharedPointer<db::Texture2D> gBufferNormalMap,
                             util::SharedPointer<db::Texture3D> indirectLightPositions,
                             const GPUBuffer& samplingPatternBuffer);

      void createProxyLightIndices();

      sh::ComputeShaderHandle m_calculateAreaLightTube;
      sh::ComputeShaderHandle m_specularCacheXCreation;
      sh::ComputeShaderHandle m_specularCacheYCreation;
      sh::ComputeShaderHandle m_specularCacheOffsetCreation;
      sh::RenderShaderHandle m_specularCacheCreation;
      sh::RenderShaderHandle m_specularLightMapCreation;
 
      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      GLuint m_pointVAO;
      GPUBuffer m_proxyLightTubeBuffer;
      GPUBuffer m_proxyLightBuffer;
      GPUBuffer m_cachePositions;
      util::SharedPointer<db::Texture2D> m_specularProxyLightOffsets;
      util::SharedPointer<db::Texture2D> m_specularLightMap;
      Renderquad m_specularProxyLightQuad;

      unsigned int m_reflectiveShadowMapNumber;
    };
  }
}

#endif
