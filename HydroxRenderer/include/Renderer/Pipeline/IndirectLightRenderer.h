#ifndef INDIRECTLIGHTRENDERER_H_
#define INDIRECTLIGHTRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

#include <Shader/ShaderContainer.h>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Buffer/UBO.h"
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

    class IndirectLightRenderer
    {
    public:

      IndirectLightRenderer();
      ~IndirectLightRenderer();

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer(unsigned int cacheNumber);
      void calculateIndirectLight(
        util::SharedPointer<db::Texture3D> reflectiveShadowPosMaps,
        util::SharedPointer<db::Texture3D> reflectiveShadowNormalMaps,
        util::SharedPointer<db::Texture3D> reflectiveShadowLuminousFluxMaps,
        const GPUImmutableBuffer& reflectiveShadowLightBuffer) const;

      void setBuffer(util::SharedPointer<db::Texture2D> depthBuffer);
      void unsetBuffer() const;

      void setCacheAndProxyLights() const;
      void unsetCacheAndProxyLights() const;

      util::SharedPointer<db::Texture2D> getIndirectLightMap() const;

      util::SharedPointer<db::Texture2D> getIndirectPositionMapDiffuse();
      util::SharedPointer<db::Texture2D> getIndirectLuminousFluxMapDiffuse();
      util::SharedPointer<db::Texture2D> getIndirectPositionMapSpecular();
      util::SharedPointer<db::Texture2D> getIndirectLuminousFluxMapSpecular();
      util::SharedPointer<db::Texture2D> getZBuffer();

      util::SharedPointer<db::Texture2D> getGlobalCachePositionMap();
      util::SharedPointer<db::Texture2D> getGlobalCacheNormalMap();

    private:
      
      IndirectLightRenderer(const IndirectLightRenderer&);
      IndirectLightRenderer& operator=(const IndirectLightRenderer&);

      void createSamplingPattern();

      util::SharedPointer<RenderOptions> m_options;

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<sh::ShaderContainer> m_renderShaderContainer;

      sh::RenderShaderHandle m_indirectLightShaderHandle;

      unsigned int m_bufferResolution;//to a power of two up rounded texture resolution

      UBO m_samplingPatternBuffer;

      util::SharedPointer<db::Texture2D> m_depthBuffer;

      util::SharedPointer<db::Texture2D> m_globalCachePositionBuffer;//saves all caches positions and diffuse strength of the scene
      util::SharedPointer<db::Texture2D> m_globalCacheNormalBuffer;//saves all caches normals, specular strength and exponent of the scene
      util::SharedPointer<db::Texture2D> m_globalCacheAreaBuffer;//saves all caches area

      util::SharedPointer<db::Texture2D> m_indirectLightPositionDBuffer;//the position of the indirect diffuse proxy light for the interpolation
      util::SharedPointer<db::Texture2D> m_indirectLightLuminousFluxDBuffer;//the luminous flux of the indirect diffuse proxy light for the soft shadows
      util::SharedPointer<db::Texture2D> m_indirectLightPositionGBuffer;//the position of the indirect specular proxy light for the interpolation
      util::SharedPointer<db::Texture2D> m_indirectLightLuminousFluxGBuffer;//the luminous flux of the indirect specular proxy light for the interpolation
      
      util::SharedPointer<db::Texture2D> m_zBuffer;//ubytes 0 = cache is not being used, 1 = cache is being used

      Renderquad m_indirectLightIndicesRenderQuad;
      Renderquad m_indirectLightRenderQuad;
      Renderquad m_indirectLightProxyLightCreationRenderQuad;

      util::SharedPointer<db::Texture2D> m_indirectLightMap;

      GLsync m_gBufferSync;

      GLuint m_cacheNumber;
    };
  }
}

#endif
