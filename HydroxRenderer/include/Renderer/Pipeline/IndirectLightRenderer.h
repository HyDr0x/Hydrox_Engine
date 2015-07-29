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

    class IndirectLightRenderer
    {
    public:

      IndirectLightRenderer();
      ~IndirectLightRenderer();

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer(unsigned int cacheNumber, unsigned int proxyLightTextureResolution);
      void calculateIndirectLight(
        util::SharedPointer<db::Texture3D> reflectiveShadowPosLuminousFluxMaps,
        util::SharedPointer<db::Texture3D> reflectiveShadowNormalAreaMaps,
        const GPUImmutableBuffer& reflectiveShadowLightBuffer);

      void setBuffer(util::SharedPointer<db::Texture2D> depthBuffer);
      void unsetBuffer() const;

      void setCacheAndProxyLights() const;
      void unsetCacheAndProxyLights() const;

      util::SharedPointer<db::Texture2D> getIndirectLightMap() const;

      util::SharedPointer<db::Texture2D> getIndirectPositionMap();
      util::SharedPointer<db::Texture2D> getIndirectLuminousFluxMap();
      util::SharedPointer<db::Texture2D> getZBuffer();

      const TBO& getGlobalCachePositionMap();
      const TBO& getGlobalCacheNormalMap();
      const TBO& getGlobalCacheAreaMap();

      util::SharedPointer<db::Texture2D> getSamplingDebugMap();
      const GPUBuffer& getSamplingBuffer();

    private:
      
      IndirectLightRenderer(const IndirectLightRenderer&);
      IndirectLightRenderer& operator=(const IndirectLightRenderer&);

      void createSamplingPattern();

      util::SharedPointer<RenderOptions> m_options;

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      unsigned int m_proxyLightTextureResolution;//to a power of two up rounded texture resolution

      GPUBuffer m_proxyLightCommandBuffer;
      GPUBuffer m_samplingPatternBuffer;

      util::SharedPointer<db::Texture2D> m_samplingDebugTexture;

      util::SharedPointer<db::Texture2D> m_depthBuffer;

      TBO m_globalCachePositionBuffer;//saves all caches positions and diffuse strength of the scene
      TBO m_globalCacheNormalBuffer;//saves all caches normals, specular strength and exponent of the scene
      TBO m_globalCacheAreaBuffer;//saves all caches area

      util::SharedPointer<db::Texture2D> m_indirectLightPositionBuffer;//the position of the indirect diffuse proxy light for the interpolation
      util::SharedPointer<db::Texture2D> m_indirectLightLuminousFluxBuffer;//the luminous flux of the indirect diffuse proxy light for the soft shadows

      util::SharedPointer<db::Texture2D> m_zBuffer;//ubytes 0 = cache is not being used, 1 = cache is being used

      Renderquad m_indirectLightIndicesRenderQuad;
      Renderquad m_indirectLightRenderQuad;

      util::SharedPointer<db::Texture2D> m_indirectLightMap;

      GLsync m_gBufferSync;

      GLuint m_cacheNumber;
    };
  }
}

#endif
