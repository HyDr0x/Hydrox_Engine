#ifndef INDIRECTLIGHTRENDERER_H_
#define INDIRECTLIGHTRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

#include "Renderer/Buffer/GPUBuffer.h"
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
    struct IndirectLight
    {
      util::vec4f position;
      util::vec4f luminousFlux;
    };

    class RenderOptions;

    class IndirectLightRenderer
    {
    public:

      IndirectLightRenderer();
      ~IndirectLightRenderer();

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer(unsigned int cacheNumber);
      void calculateIndirectLight(util::SharedPointer<db::Texture2D> depthMap, 
        util::SharedPointer<db::Texture2D> normalMap, 
        util::SharedPointer<db::Texture2D> materialMap, 
        util::SharedPointer<db::Texture3D> reflectiveShadowPosMaps,
        util::SharedPointer<db::Texture3D> reflectiveShadowNormalMaps,
        util::SharedPointer<db::Texture3D> reflectiveShadowLuminousFluxMaps) const;

      void setBuffer(util::SharedPointer<db::Texture2D> depthTexture);
      void unsetBuffer() const;

      util::SharedPointer<db::Texture2D> getIndirectLightMap() const;

    private:
      
      IndirectLightRenderer(const IndirectLightRenderer&);
      IndirectLightRenderer& operator=(const IndirectLightRenderer&);

      util::SharedPointer<RenderOptions> m_options;

      util::SingletonManager *m_singletonManager;

      unsigned int m_bufferResolution;//to a power of two up rounded texture resolution

      util::SharedPointer<db::Texture2D> m_globalCachePositionBuffer;//saves all caches positions and diffuse strength of the scene
      util::SharedPointer<db::Texture2D> m_globalCacheNormalBuffer;//saves all caches normals, specular strength and exponent of the scene
      util::SharedPointer<db::Texture2D> m_indirectLightPositionBuffer;//the position of the indirect light for the interpolation
      util::SharedPointer<db::Texture2D> m_indirectLightLuminousFluxBuffer;//the luminous flux of the indirect light for the interpolation
      
      util::SharedPointer<db::Texture2D> m_clearTexture;//clears the zBuffer with zeros
      util::SharedPointer<db::Texture2D> m_zBuffer;//ubytes 0 = cache is not being used, 1 = cache is being used

      Renderquad m_indirectLightIndicesRenderQuad;
      Renderquad m_indirectLightRenderQuad;

      util::SharedPointer<db::Texture3D> m_frameCacheIndexMap;//saves all 24 indices per pixel of the frame buffer
      util::SharedPointer<db::Texture2D> m_indirectLightMap;

      util::ResourceHandle m_indirectLightShaderHandle;
      util::ResourceHandle m_indirectLightInterpolationShaderHandle;

      GLuint m_cacheNumber;
    };
  }
}

#endif
