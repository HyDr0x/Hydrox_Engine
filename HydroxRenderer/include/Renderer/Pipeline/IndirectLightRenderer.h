#ifndef INDIRECTLIGHTRENDERER_H_
#define INDIRECTLIGHTRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

#include "Renderer/Buffer/GPUBuffer.h"
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

      void setBuffer() const;
      void unsetBuffer() const;

      util::SharedPointer<db::Texture2D> getIndirectLightMap() const;

    private:
      
      IndirectLightRenderer(const IndirectLightRenderer&);
      IndirectLightRenderer& operator=(const IndirectLightRenderer&);

      RenderOptions *m_options;

      util::SingletonManager *m_singletonManager;

      GPUBuffer m_indirectLightDataBuffer;//the indirect light for the interpolation
      GPUBuffer m_frameCacheIndexBuffer;//saves all 24 indices per pixel of the frame buffer
      GPUBuffer m_globalCacheBuffer;//saves all caches of the scene
      GPUBuffer m_zBuffer;//uints 0 = cache is not being used, 1 = cache is being 

      Renderquad m_indirectLightRenderQuad;

      util::SharedPointer<db::Texture2D> m_indirectLightMap;

      util::ResourceHandle m_indirectLightShaderHandle;
      util::ResourceHandle m_indirectLightInterpolationShaderHandle;

      GLuint m_cacheNumber;
    };
  }
}

#endif
