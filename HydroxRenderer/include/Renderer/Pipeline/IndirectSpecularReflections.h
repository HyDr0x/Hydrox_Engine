#ifndef INDIRECTSPECULARREFLECTIONS_H_
#define INDIRECTSPECULARREFLECTIONS_H_

#include <list>

#include <GL/glew.h>

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
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        const GPUBuffer& samplingPatternBuffer);

      const GPUBuffer& getSpecularProxyLights() const;
      const GPUBuffer& getSpecularCacheMap() const;
      util::SharedPointer<db::Texture2D> getSpecularProxyIndexMap() const;
      util::SharedPointer<db::Texture2D> getSpecularProxyOffsets() const;

    private:
      
      IndirectSpecularReflections(const IndirectSpecularReflections&);
      IndirectSpecularReflections& operator=(const IndirectSpecularReflections&);

      void createProxyLightOffsetBuffer(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                                        util::SharedPointer<db::Texture2D> gBufferNormalMap,
                                        util::SharedPointer<db::Texture3D> indirectLightPositions,
                                        const GPUBuffer& samplingPatternBuffer);

      void createProxyLights(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                             util::SharedPointer<db::Texture2D> gBufferNormalMap,
                             util::SharedPointer<db::Texture3D> indirectLightPositions,
                             const GPUBuffer& samplingPatternBuffer);

      void createProxyLightIndices();

      sh::RenderShaderHandle m_proxyLightOffsetShaderHandle;
      sh::RenderShaderHandle m_proxyLightCreationShaderHandle;
 
      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      GLuint m_pointVAO;
      GPUBuffer m_proxyLightBoundingBoxBuffer;
      GPUBuffer m_proxyLightBuffer;
      GPUBuffer m_cacheBuffer;
      util::SharedPointer<db::Texture2D> m_specularProxyLightOffsets;
      util::SharedPointer<db::Texture2D> m_specularProxyIndices;
      Renderquad m_specularProxyLightQuad;

      unsigned int m_reflectiveShadowMapNumber;
    };
  }
}

#endif
