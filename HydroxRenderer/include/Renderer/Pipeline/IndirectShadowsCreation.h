#ifndef INDIRECTSHADOWSCREATION_H_
#define INDIRECTSHADOWSCREATION_H_

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

    class IndirectShadowsCreation
    {
    public:

      IndirectShadowsCreation();
      ~IndirectShadowsCreation();

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer(unsigned int reflectiveShadowMapNumber);

      void generateImperfectShadowMap(
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        util::SharedPointer<db::Texture3D> indirectLightNormals,
        const TBO& globalCachePositionBuffer, 
        const TBO& globalCacheNormalBuffer,
        const TBO& adaptiveSamplingPositionBuffer,
        const TBO& adaptiveSamplingNormalBuffer,
        const GPUBuffer& samplingPatternBuffer,
        const GPUBuffer& commandBuffer,
        unsigned int cacheNumber);

      void generateIndirectLightsShadowMap(
        util::SharedPointer<db::Texture2D> gBufferDepthMap,
        util::SharedPointer<db::Texture2D> gBufferNormalMap,
        util::SharedPointer<db::Texture3D> indirectLightPositions,
        util::SharedPointer<db::Texture3D> indirectLightNormals,
        const GPUBuffer& samplingPatternBuffer);

      void viewMatrixCreation(util::SharedPointer<db::Texture3D> indirectLightPositions, util::SharedPointer<db::Texture3D> indirectLightNormals);

      void setBackprojectionMap(unsigned int lightIndex);
      void unsetBackprojectionMap();

      util::SharedPointer<db::Texture3D> getIndirectLightsShadowMaps() const;
      util::SharedPointer<db::Texture2D> getIndirectShadowMap() const;
      std::vector<util::SharedPointer<db::Texture3D>> getIndirectPushPullShadowMap() const;

      util::SharedPointer<db::Texture3D> getBackprojectionDepthMaps() const;
      util::SharedPointer<db::Texture3D> getBackprojectionPositionMaps() const;
      util::SharedPointer<db::Texture3D> getBackprojectionNormalMaps() const;

    private:
      
      IndirectShadowsCreation(const IndirectShadowsCreation&);
      IndirectShadowsCreation& operator=(const IndirectShadowsCreation&);

      void generateShadowMap(util::SharedPointer<db::Texture2D> gBufferDepthMap,
                             util::SharedPointer<db::Texture3D> indirectLightPositions,
                             util::SharedPointer<db::Texture3D> indirectLightNormals,
                             const GPUBuffer& samplingPatternBuffer);

      void bilateralInterleavedBlurShadowMap(util::SharedPointer<db::Texture2D> gBufferDepthMap, util::SharedPointer<db::Texture2D> gBufferNormalMap);

      void pullPush();

      void bilateralBlurShadowMap(util::SharedPointer<db::Texture2D> gBufferDepthMap, util::SharedPointer<db::Texture2D> gBufferNormalMap);

      sh::RenderShaderHandle m_indirectShadowMapsShaderHandle;
      sh::RenderShaderHandle m_pullShaderHandle;
      sh::RenderShaderHandle m_pushShaderHandle;
      sh::RenderShaderHandle m_blurShaderHandle;
      sh::ComputeShaderHandle m_viewMatrixCreationHandle;
      sh::ComputeShaderHandle m_indirectShadowMapCreationCompShaderHandle;

      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      unsigned int m_reflectiveShadowMapNumber;

      GLuint m_pointVAO;
      util::SharedPointer<db::Texture2D> m_VALQuaternions;
      util::SharedPointer<db::Texture3D> m_indirectLightShadowMaps;
      std::vector<util::SharedPointer<db::Texture3D>> m_indirectLightPushPullShadowMapsReference;
      std::vector<util::SharedPointer<db::Texture3D>> m_indirectLightPushPullShadowMaps;
      Renderquad m_indirectLightShadowMapsQuad;
 
      unsigned int m_ping, m_pong;
      util::SharedPointer<db::Texture2D> m_indirectShadowMap[2];
      Renderquad m_indirectShadowMapQuad;

      util::SharedPointer<db::Texture3D> m_geometryBackprojectionDepthMap;
      util::SharedPointer<db::Texture3D> m_geometryBackprojectionPositionMap;
      util::SharedPointer<db::Texture3D> m_geometryBackprojectionNormalMap;
      Renderquad m_backprojectionRenderQuad;
    };
  }
}

#endif
