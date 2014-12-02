#include "Renderer/Pipeline/RenderManager.h"

#include <XBar/LightContainer.h>
#include <XBar/BillboardContainer.h>
#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Pipeline/RenderOptions.h"
#include "Renderer/Pipeline/RenderShaderContainer.h"

namespace he
{
  namespace renderer
  {
    RenderManager::RenderManager() : m_skyboxRendering(false), m_wireframe(false)
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

    void RenderManager::initialize(util::SingletonManager *singletonManager)
    {
      m_singletonManager = singletonManager;

      m_options = m_singletonManager->getService<RenderOptions>();

      m_geometryRasterizer.initialize(m_singletonManager);
      m_gBuffer.initialize(m_singletonManager);
      m_billboardRenderer.initialize(m_singletonManager);
      m_spriteRenderer.initialize(m_singletonManager);
      m_stringRenderer.initialize(m_singletonManager);
      m_lightRenderer.initialize(m_singletonManager);
      m_indirectLightRenderer.initialize(m_singletonManager);
      m_particleRenderer.initialize(m_singletonManager);
      m_finalCompositing.initialize(m_singletonManager);

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 4 + sizeof(util::vec4f) + sizeof(GLfloat) * 2 + sizeof(GLuint) * 2, GL_DYNAMIC_DRAW);
    }

    void RenderManager::setViewPort(GLuint width, GLuint height, GLfloat near, GLfloat far)
    {
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f), sizeof(GLuint), &width);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + sizeof(GLuint), sizeof(GLuint), &height);

      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + 2 * sizeof(GLuint), sizeof(GLfloat), &near);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + 2 * sizeof(GLuint) + sizeof(GLfloat), sizeof(GLfloat), &far);
    }

    void RenderManager::render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::vec3f& cameraPosition, float near, float far)
    {
      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
      util::vec4f eyeVec = util::vec4f(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

      m_cameraParameterUBO.setData(0, sizeof(util::Matrix<float, 4>), &viewMatrix[0][0]);
      m_cameraParameterUBO.setData(1 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &projectionMatrix[0][0]);
      m_cameraParameterUBO.setData(2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &viewProjectionMatrix[0][0]);
      m_cameraParameterUBO.setData(3 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &viewProjectionMatrix.invert()[0][0]);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>), sizeof(util::vec4f), &eyeVec[0]);
      m_cameraParameterUBO.uploadData();
      m_cameraParameterUBO.bindBuffer(0);

      {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        m_gBuffer.clear();
        m_lightRenderer.clear();
        
        if(m_wireframe)
        {
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        m_geometryRasterizer.updateBuffer();
        m_indirectLightRenderer.updateBuffer(m_geometryRasterizer.getGlobalCacheNumber());

        m_geometryRasterizer.frustumCulling(-1, VIEWPASS);

        m_gBuffer.setGBuffer();
        m_geometryRasterizer.rasterizeGeometry();
        m_gBuffer.unsetGBuffer();

        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        m_indirectLightRenderer.setBuffer(m_gBuffer.getDepthTexture());
        m_geometryRasterizer.rasterizeIndexGeometry();
        m_indirectLightRenderer.unsetBuffer();
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        if(m_wireframe)
        {
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        m_lightRenderer.updateBuffer();
        
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.1f, 4.0f);
        glViewport(0, 0, m_options->shadowMapWidth, m_options->shadowMapWidth);
        for(unsigned int i = 0; i < m_lightRenderer.getShadowLightNumber(); i++)
        {
          m_lightRenderer.setShadowMap(4, i);
          m_geometryRasterizer.frustumCulling(i, SHADOWPASS);
          m_geometryRasterizer.generateShadowMap(i, SHADOWPASS);
          m_lightRenderer.unsetShadowMap(4);
        }

        for(unsigned int i = 0; i < m_lightRenderer.getReflectiveShadowLightNumber(); i++)
        {
          m_lightRenderer.setReflectiveShadowMap(4, i);
          m_geometryRasterizer.frustumCulling(i, REFLECTIVESHADOWPASS);
          m_geometryRasterizer.generateShadowMap(i, REFLECTIVESHADOWPASS);
          m_lightRenderer.unsetReflectiveShadowMap(4);
        }
        glViewport(0, 0, m_options->width, m_options->height);
        glDisable(GL_POLYGON_OFFSET_FILL);

        m_lightRenderer.render(m_gBuffer.getDepthTexture(), m_gBuffer.getNormalTexture(), m_gBuffer.getMaterialTexture());
        
        m_indirectLightRenderer.calculateIndirectLight(
          m_gBuffer.getDepthTexture(), 
          m_gBuffer.getNormalTexture(), 
          m_gBuffer.getMaterialTexture(), 
          m_lightRenderer.getReflectiveShadowPosMaps(),
          m_lightRenderer.getReflectiveShadowNormalMaps(),
          m_lightRenderer.getReflectiveShadowLuminousFluxMaps());
        
        m_gBuffer.setGBuffer();

        m_billboardRenderer.render();

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

        m_gBuffer.unsetGBuffer();
      }
      
      //m_gBuffer.getColorTexture()
      //m_gBuffer.getNormalTexture()
      //m_lightRenderer.getReflectiveShadowPosMaps()->convertToTexture2D(0)
      //m_lightRenderer.getReflectiveShadowNormalMaps()->convertToTexture2D(0)
      //m_lightRenderer.getReflectiveShadowLuminousFluxMaps()->convertToTexture2D(0)
      //m_lightRenderer.getShadowMaps()->convertToTexture2D(0)
      m_finalCompositing.composeImage(m_indirectLightRenderer.getIndirectLightMap(), m_lightRenderer.getLightTexture(), m_indirectLightRenderer.getIndirectLightMap());
      
      m_spriteRenderer.render();
      m_stringRenderer.render();
      
      m_cameraParameterUBO.unBindBuffer(0);
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

    void RenderManager::addRenderComponent(const xBar::StaticGeometryContainer& staticGeometry)
    {
      m_geometryRasterizer.addRenderComponent(staticGeometry);
    }

    void RenderManager::addRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry)
    {
      m_geometryRasterizer.addRenderComponent(skinnedGeometry);
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

    void RenderManager::removeRenderComponent(const xBar::StaticGeometryContainer& staticGeometry)
    {
      m_geometryRasterizer.removeRenderComponent(staticGeometry);
    }

    void RenderManager::removeRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry)
    {
      m_geometryRasterizer.removeRenderComponent(skinnedGeometry);
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
