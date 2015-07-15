#ifndef DELAUNAYTRIANGULATION_H_
#define DELAUNAYTRIANGULATION_H_

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

    class DelaunayTriangulation
    {
    public:

      DelaunayTriangulation();
      ~DelaunayTriangulation();

      void initialize(util::SingletonManager *singletonManager);

      void updateBuffer(unsigned int cacheNumber);

      void calculateDelaunayTriangulation(
        util::SharedPointer<db::Texture3D> backprojectionZBuffer,
        util::SharedPointer<db::Texture3D> m_backprojectionPosition, 
        util::SharedPointer<db::Texture3D> m_backprojectionNormal, 
        const TBO& globalCachePositionBuffer, 
        const GPUImmutableBuffer& reflectiveShadowLightsBuffer);

      void showVoronoiDiagram();

      util::SharedPointer<db::Texture2D> getVoronoiDiagram();

      const TBO& getAdaptiveSamplePositionBuffer();
      const TBO& getAdaptiveSampleNormalBuffer();

      const GPUBuffer& getCommandBuffer();

    private:
      
      DelaunayTriangulation(const DelaunayTriangulation&);
      DelaunayTriangulation& operator=(const DelaunayTriangulation&);

      void generateProjectedSiteBuffer(util::SharedPointer<db::Texture3D> backprojectionZBuffer, const TBO& globalCachePositionBuffer, const GPUImmutableBuffer& reflectiveShadowLightsBuffer);
      void generateVoronoiDiagram();
      void generateTriangles(util::SharedPointer<db::Texture3D> m_backprojectionPosition, util::SharedPointer<db::Texture3D> m_backprojectionNormal);

      util::SharedPointer<RenderOptions> m_options;
      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      sh::RenderShaderHandle m_siteProjectionShaderHandle;
      sh::RenderShaderHandle m_voronoiCreationShaderHandle;
      sh::ComputeShaderHandle m_sampleOffsetShaderHandle;
      sh::ComputeShaderHandle m_sampleOffsetSummationShaderHandle;
      sh::ComputeShaderHandle m_delaunayTriangulationShaderHandle;
      sh::RenderShaderHandle m_showVoronoiDiagramShaderHandle;

      GLuint m_siteVAO;

      GPUBuffer m_sampleCommandBuffer;
      TBO m_adaptiveCacheSamplesPosition;
      TBO m_adaptiveCacheSamplesNormals;
      TBO m_projectedSites;
      util::SharedPointer<db::Texture1D> m_voronoiColorTexture;
      util::SharedPointer<db::Texture1D> m_noiseTexture;
      util::SharedPointer<db::Texture1D> m_sampleNumberOffset;
      util::SharedPointer<db::Texture2D> m_voronoiDiagram[2];
      unsigned int m_ping, m_pong;
      Renderquad m_siteProjectionRenderQuad;
      Renderquad m_voronoiRenderQuad;

      GLuint m_cacheNumber;
    };
  }
}

#endif
