#include "Renderer/Pipeline/RenderManager.h"

#include <Shader/ShaderContainer.h>

#include <DataBase/Light.h>
#include <DataBase/Texture2D.h>

#include <XBar/LightContainer.h>
#include <XBar/BillboardContainer.h>
#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include <Utilities/PrimitiveGenerator/SphereGenerator.h>
#include <Utilities/PrimitiveGenerator/DiscGenerator.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    RenderManager::RenderManager() : m_skyboxRendering(false), m_wireframe(false), m_debugTexture(0)
    {
    }

    RenderManager::~RenderManager()
    {
    }

    void RenderManager::setClearColor(he::util::vec4f color)
    {
      glClearColor(color[0], color[1], color[2], color[3]);
      m_gBuffer.setClearColor(color);
    }

    void RenderManager::resizeRenderWindow(unsigned int width, unsigned int height)
    {
      m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

      glViewport(0, 0, width, height);
    }

    void RenderManager::setWindingOrder(GLenum windingOrder) const
    {
      glFrontFace(windingOrder);
    }

    void RenderManager::enableBackfaceCulling(GLenum cullMode)
    {
      glEnable(GL_CULL_FACE);

      m_cullMode = cullMode;

      m_cullModeNotBack = false;
      if(m_cullMode != GL_BACK)
      {
        m_cullModeNotBack = true;
      }
    }

    void RenderManager::disableBackfaceCulling()
    {
      glDisable(GL_CULL_FACE);
      m_cullModeNotBack = false;
    }

    void RenderManager::setWireframe(bool wireFrame)
    {
      m_wireframe = wireFrame;
    }

    void RenderManager::enableSkybox(util::ResourceHandle skyboxTextureHandles[6])
    {
      m_skyboxRendering = true;
      m_skyboxRenderer.initialize(m_singletonManager, skyboxTextureHandles);
    }

    void RenderManager::disableSkybox()
    {
      m_skyboxRendering = false;
    }

    void RenderManager::showDebugTexture(unsigned int debugTexture)
    {
      m_debugTexture = debugTexture;
    }

    void RenderManager::initialize(util::SingletonManager *singletonManager)
    {
      m_singletonManager = singletonManager;

      m_options = m_singletonManager->getService<RenderOptions>();
      m_cameraManager = m_singletonManager->getService<CameraManager>();

      m_gBuffer.initialize(m_singletonManager);
      m_geometryRasterizer.initialize(m_singletonManager, m_gBuffer.getNormalTexture(), m_gBuffer.getMaterialTexture());
      m_billboardRenderer.initialize(m_singletonManager);
      m_spriteRenderer.initialize(m_singletonManager);
      m_stringRenderer.initialize(m_singletonManager);
      m_lightRenderer.initialize(m_singletonManager);
      m_indirectSpecularRenderer.initialize(m_singletonManager);
      m_indirectShadowsCreation.initialize(m_singletonManager);
      m_delaunayTriangulation.initialize(m_singletonManager);
      m_particleRenderer.initialize(m_singletonManager);
      m_finalCompositing.initialize(m_singletonManager);
      m_tonemapper.initialize(m_singletonManager);

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 4 + sizeof(util::vec4f) + sizeof(GLfloat) * 2 + sizeof(GLuint) * 2, GL_DYNAMIC_DRAW);
    }

    void RenderManager::setViewPort()
    {
      CameraData& cameraData = m_cameraManager->getCamera(0);

      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f), sizeof(GLuint), &cameraData.width);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + sizeof(GLuint), sizeof(GLuint), &cameraData.height);

      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + 2 * sizeof(GLuint), sizeof(GLfloat), &cameraData.znear);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + 2 * sizeof(GLuint) + sizeof(GLfloat), sizeof(GLfloat), &cameraData.zfar);
    }

    void RenderManager::render()
    {
      CameraData& cameraData = m_cameraManager->getCamera(0);

      m_cameraParameterUBO.setData(0, sizeof(util::Matrix<float, 4>), &cameraData.viewMatrix[0][0]);
      m_cameraParameterUBO.setData(1 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &cameraData.projectionMatrix[0][0]);
      m_cameraParameterUBO.setData(2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &cameraData.viewProjectionMatrix[0][0]);
      m_cameraParameterUBO.setData(3 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &cameraData.invViewProjectionMatrix[0][0]);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>), sizeof(util::vec4f), &cameraData.cameraPosition[0]);
      m_cameraParameterUBO.uploadData();
      m_cameraParameterUBO.bindBuffer(0);

      {//everything in here should be packed in an render pass interface and the blocks like gbuffer etc. in a technique interface which gets passed to a render pass 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_gBuffer.clear();
        m_lightRenderer.clear();

        bool globalIllumination;
        {
          //CPUTIMER("BufferUpdateCPUTimer", 0)
          //GPUTIMER("BufferUpdateOGLTimer", 1)

          m_geometryRasterizer.updateBuffer();
          m_lightRenderer.updateBuffer();

          globalIllumination = m_lightRenderer.getReflectiveShadowLightNumber() > 0 && m_options->globalIllumination;

          if(globalIllumination)
          {
            if(m_options->indirectShadows)
            {
              m_indirectShadowsCreation.updateBuffer(m_lightRenderer.getReflectiveShadowLightNumber());
              m_indirectShadowsCreation.updateOccluderBuffer(m_geometryRasterizer.getGlobalOccluderNumber());
              //m_delaunayTriangulation.updateBuffer(m_geometryRasterizer.getGlobalOccluderNumber());
            }

            m_indirectSpecularRenderer.updateBuffer(m_lightRenderer.getReflectiveShadowLightNumber(), m_geometryRasterizer.getGlobalOccluderNumber());
          }
        }

        if(m_wireframe)
        {
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        m_geometryRasterizer.frustumCulling(-1, VIEWPASS);
        
        {
          //CPUTIMER("GBufferCPUTimer", 0)
          //GPUTIMER("GBufferOGLTimer", 1)

          m_gBuffer.setGBuffer();
          m_indirectShadowsCreation.setOccluderBuffer();
          m_geometryRasterizer.rasterizeGeometry();
          m_indirectShadowsCreation.unsetOccluderBuffer();

          m_billboardRenderer.render();

          m_gBuffer.unsetGBuffer();
        }

        {
          //CPUTIMER("ShadowMapsCPUTimer", 0)
          //GPUTIMER("ShadowMapsOGLTimer", 1)

          glViewport(0, 0, m_options->shadowMapWidth, m_options->shadowMapWidth);
          glEnable(GL_POLYGON_OFFSET_FILL);
          glPolygonOffset(1.1f, 4.0f);
          for(unsigned int i = 0; i < m_lightRenderer.getShadowLightNumber(); i++)
          {
            m_lightRenderer.setShadowMap(4, i);
            m_geometryRasterizer.frustumCulling(i, SHADOWPASS);
            m_geometryRasterizer.generateShadowMap(i);
            m_lightRenderer.unsetShadowMap(4);
          }

          for(unsigned int i = 0; i < m_lightRenderer.getReflectiveShadowLightNumber(); i++)
          {
            m_lightRenderer.setReflectiveShadowMap(4, i);
            m_geometryRasterizer.frustumCulling(i, REFLECTIVESHADOWPASS);
            m_geometryRasterizer.generateReflectiveShadowMap(i);
            m_lightRenderer.unsetReflectiveShadowMap(4, i);
          }

          glDisable(GL_POLYGON_OFFSET_FILL);
          glViewport(0, 0, m_options->width, m_options->height);
        }

        if(m_wireframe)
        {
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        m_lightRenderer.render(m_gBuffer.getDepthTexture(), m_gBuffer.getColorTexture(), m_gBuffer.getNormalTexture(), m_gBuffer.getMaterialTexture());
        
        if(globalIllumination)
        {
          m_gBuffer.calculateLinearDepthBuffer();

          //m_gBuffer.useTestGBuffer();
          m_indirectSpecularRenderer.gBufferDownsampling(
            m_gBuffer.getDepthTexture(),
            m_gBuffer.getLinearDepthTexture(),
            m_gBuffer.getNormalTexture(),
            m_gBuffer.getVertexNormalTexture(),
            m_gBuffer.getMaterialTexture());
          
          if(m_options->indirectShadows)
          {
            //m_lightRenderer.getReflectiveShadowLights().bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

            //m_indirectShadowsCreation.viewMatrixCreation(m_lightRenderer.getReflectiveShadowPosMaps(), m_lightRenderer.getReflectiveShadowNormalAreaMaps());

            //for(unsigned int i = 0; i < m_lightRenderer.getReflectiveShadowLightNumber(); i++)
            //{
            //  m_indirectShadowsCreation.setBackprojectionMap(i);
            //  m_geometryRasterizer.generateIndirectBackprojectionMap(i);
            //  m_indirectShadowsCreation.unsetBackprojectionMap();
            //}

            //m_lightRenderer.getReflectiveShadowLights().unbindBuffer(GL_SHADER_STORAGE_BUFFER);

            //m_delaunayTriangulation.calculateDelaunayTriangulation(
            //  m_indirectShadowsCreation.getBackprojectionDepthMaps(),
            //  m_indirectShadowsCreation.getBackprojectionPositionMaps(),
            //  m_indirectShadowsCreation.getBackprojectionNormalMaps(),
            //  m_indirectShadowsCreation.getOccluderNormalCoordinates(),
            //  m_lightRenderer.getReflectiveShadowLights());

            m_indirectShadowsCreation.generateImperfectShadowMap(
              m_lightRenderer.getReflectiveShadowPosMaps(),
              m_lightRenderer.getReflectiveShadowNormalAreaMaps(),
              m_delaunayTriangulation.getAdaptiveSamplePositionBuffer(),
              m_delaunayTriangulation.getAdaptiveSampleNormalBuffer(),
              m_delaunayTriangulation.getCommandBuffer());//generates the imperfect shadow maps and fills holes with pull push

            //m_indirectShadowsCreation.generateIndirectLightsShadowMap(
            //  m_indirectSpecularRenderer.getDebugGBufferHalfResDepthMap(),
            //  m_indirectSpecularRenderer.getDebugGBufferHalfResNormalMap(),
            //  m_lightRenderer.getReflectiveShadowPosMaps(),
            //  m_lightRenderer.getReflectiveShadowNormalAreaMaps());//does blurring and visibility calculation of the indirect light sources
          }

          m_indirectSpecularRenderer.generateReflectionCaches(
            m_gBuffer.getDepthTexture(),
            m_gBuffer.getLinearDepthTexture(),
            m_gBuffer.getNormalTexture(),
            m_gBuffer.getVertexNormalTexture(),
            m_gBuffer.getMaterialTexture(),
            m_lightRenderer.getReflectiveShadowPosMaps(),
            m_lightRenderer.getReflectiveShadowNormalAreaMaps(),
            m_lightRenderer.getReflectiveShadowLuminousFluxMaps(),
            m_indirectShadowsCreation.getIndirectLightsShadowMaps(),
            m_indirectShadowsCreation.getIndirectShadowVALQuaternions(),
            m_indirectShadowsCreation.getOccluderNormalCoordinates());
        }

        m_gBuffer.setColorBuffer();
        m_geometryRasterizer.rasterizeNoLightingGeometry();

        if(m_skyboxRendering)
        {
          if(m_cullModeNotBack)
          {
            glCullFace(GL_BACK);
          }

          m_skyboxRenderer.render();

          if(m_cullModeNotBack)
          {
            glCullFace(m_cullMode);
          }
        }

        m_gBuffer.unsetColorBuffer();
      }

      switch(m_debugTexture)
      {
      case 1:
        m_finalCompositing.renderDebugOutput(m_gBuffer.getColorTexture());
        //m_finalCompositing.renderDebugOutput(m_lightRenderer.getLightTexture());
        //m_finalCompositing.renderDebugOutput(m_indirectShadowsCreation.getIndirectPushPullShadowMap()[0]->convertToTexture2D(0));
        break;
      case 2:
        //m_finalCompositing.renderDebugOutput(m_indirectShadowsCreation.getIndirectPushPullShadowMap()[1]->convertToTexture2D(0));
        m_finalCompositing.renderDebugOutput(m_gBuffer.getMaterialTexture());
        break;
      case 3:
        //m_finalCompositing.renderDebugOutput(m_indirectShadowsCreation.getIndirectPushPullShadowMap()[2]->convertToTexture2D(0));
        m_finalCompositing.renderDebugOutput(m_gBuffer.getNormalTexture());
        break;
      case 4:
        //m_finalCompositing.renderDebugOutput(m_indirectShadowsCreation.getIndirectLightsShadowMaps()->convertToTexture2D(0));
        m_finalCompositing.renderDebugOutput(m_gBuffer.getVertexNormalTexture());
        break;
      case 5:
        //m_finalCompositing.renderDebugOutput(m_indirectShadowsCreation.getIndirectShadowMap());
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getDownsampledSpecularLightMap());
        break;
      case 6:
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getSpecularLightMap());
        break;
      case 7:
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getDebugCacheNormals());
        break;
      case 8:
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getDebugVertexNormals());
        break;
      case 9:
        m_finalCompositing.renderIntegerDebugOutput(m_indirectSpecularRenderer.getDebugCacheBlockNumber());
        break;
      case 10:
        m_finalCompositing.renderIntegerDebugOutput(m_indirectSpecularRenderer.getDebugVertexBlockNumber());
        break;
      case 11:
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getDebugGBufferHalfResDepthMap());
        break;
      case 12:
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getDebugGBufferHalfResLinearDepthMap());
        break;
      case 13:
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getDebugGBufferHalfResNormalMap());
        break;
      case 14:
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getDebugGBufferHalfResMaterialMap());
        break;
      case 15:
        //m_delaunayTriangulation.showVoronoiDiagram();
        m_finalCompositing.renderDebugOutput(m_gBuffer.getColorTexture());
        break;
      case 16:
        //m_finalCompositing.renderDebugOutput(m_lightRenderer.getReflectiveShadowLuminousFluxMaps()->convertToTexture2D(0));
        m_finalCompositing.renderDebugOutput(m_indirectSpecularRenderer.getSpecularLightMap());
        break;
      case 0:
      default:
        m_finalCompositing.composeImage(m_gBuffer.getColorTexture(), m_lightRenderer.getLightTexture(), m_indirectSpecularRenderer.getSpecularLightMap());
        m_finalCompositing.renderDebugOutput(m_finalCompositing.getCombinedTexture());
      }
      
      m_spriteRenderer.render();
      m_stringRenderer.render();
      
      m_cameraParameterUBO.unbindBuffer(0);
    }

    void RenderManager::addRenderComponent(const db::Sprite *sprite)
    {
      m_spriteRenderer.addRenderComponent(sprite);
    }

    void RenderManager::addRenderComponent(const StringTexture2D *string)
    {
      m_stringRenderer.addRenderComponent(string);
    }

    void RenderManager::addRenderComponent(const xBar::BillboardContainer& billboard)
    {
      m_billboardRenderer.addRenderComponent(billboard);
    }

    void RenderManager::addRenderComponent(util::SharedPointer<const xBar::StaticGeometryContainer> staticGeometry)
    {
      m_geometryRasterizer.addRenderComponent(staticGeometry.dynamic_pointer_cast<const xBar::IGeometryContainer>());
    }

    void RenderManager::addRenderComponent(util::SharedPointer<const xBar::SkinnedGeometryContainer> skinnedGeometry)
    {
      m_geometryRasterizer.addRenderComponent(skinnedGeometry.dynamic_pointer_cast<const xBar::IGeometryContainer>());
    }

    void RenderManager::addRenderComponent(const xBar::LightContainer& light)
    {
      m_lightRenderer.addLight(light);
    }

    void RenderManager::addRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter)
    {
      m_particleRenderer.addRenderComponent(particleEmitter);
    }

    void RenderManager::removeRenderComponent(const db::Sprite *sprite)
    {
      m_spriteRenderer.removeRenderComponent(sprite);
    }

    void RenderManager::removeRenderComponent(const StringTexture2D *string)
    {
      m_stringRenderer.removeRenderComponent(string);
    }

    void RenderManager::removeRenderComponent(const xBar::BillboardContainer& billboard)
    {
      m_billboardRenderer.removeRenderComponent(billboard);
    }

    void RenderManager::removeRenderComponent(util::SharedPointer<const xBar::StaticGeometryContainer> staticGeometry)
    {
      m_geometryRasterizer.removeRenderComponent(staticGeometry.dynamic_pointer_cast<const xBar::IGeometryContainer>());
    }

    void RenderManager::removeRenderComponent(util::SharedPointer<const xBar::SkinnedGeometryContainer> skinnedGeometry)
    {
      m_geometryRasterizer.removeRenderComponent(skinnedGeometry.dynamic_pointer_cast<const xBar::IGeometryContainer>());
    }

    void RenderManager::removeRenderComponent(const xBar::LightContainer& light)
    {
      m_lightRenderer.removeLight(light);
    }

    void RenderManager::removeRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter)
    {
      m_particleRenderer.removeRenderComponent(particleEmitter);
    }
  }
}
