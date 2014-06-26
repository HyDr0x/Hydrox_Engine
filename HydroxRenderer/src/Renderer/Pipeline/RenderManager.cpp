#include "Renderer/Pipeline/RenderManager.h"

#include <XBar/LightContainer.h>
#include <XBar/BillboardContainer.h>
#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Pipeline/RenderOptions.h"

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

    void RenderManager::setClearColor(he::util::Vector<float, 4> color) const
    {
      glClearColor(color[0], color[1], color[2], color[3]);
    }

    void RenderManager::resizeRenderWindow(unsigned int width, unsigned int height)
    {
      m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

      glViewport(0, 0, width, height);
    }

    void RenderManager::setBackfaceCulling(GLenum cullingMode) const
    {
      glFrontFace(cullingMode);
    }

    void RenderManager::setWireframe(bool wireFrame)
    {
      m_wireframe = wireFrame;
      if(wireFrame)
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      else
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
    }

    void RenderManager::enableSkybox(util::SingletonManager *singletonManager, util::ResourceHandle skyboxShaderHandle, util::ResourceHandle skyboxTextureHandles[6])
    {
      m_skyboxRendering = true;
      m_skyboxRenderer.initialize(singletonManager, skyboxShaderHandle, skyboxTextureHandles);
    }

    void RenderManager::disableSkybox()
    {
      m_skyboxRendering = false;
    }

    void RenderManager::initialize(const RenderOptions& options, util::SingletonManager *singletonManager, unsigned char maxLayer,
      util::ResourceHandle billboardShaderHandle, 
      util::ResourceHandle spriteShaderHandle, 
      util::ResourceHandle stringShaderHandle,
      util::ResourceHandle frustumCullingShaderHandle,
      util::ResourceHandle offscreenBufferShaderHandle,
      util::ResourceHandle directLightShaderHandle,
      util::ResourceHandle combineShaderHandle)
    {
      m_singletonManager = singletonManager;

      m_options = options;

      m_offscreenBufferShaderHandle = offscreenBufferShaderHandle;
      m_combineShaderHandle = combineShaderHandle;

      m_geometryRasterizer.initialize(m_options, m_singletonManager, frustumCullingShaderHandle, frustumCullingShaderHandle);
      m_gBuffer.initialize(m_options, m_singletonManager);
      m_billboardRenderer.initialize(m_singletonManager, billboardShaderHandle);
      m_spriteRenderer.initialize(m_singletonManager, spriteShaderHandle, maxLayer);
      m_stringRenderer.initialize(m_singletonManager, stringShaderHandle, maxLayer);
      m_lightRenderer.initialize(m_options, m_singletonManager, directLightShaderHandle);
      m_particleRenderer.initialize(m_singletonManager, billboardShaderHandle);//dummy shader 

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 4 + sizeof(util::Vector<float, 4>) + sizeof(GLfloat) * 2 + sizeof(GLuint) * 2, GL_DYNAMIC_DRAW);
    }

    void RenderManager::setViewPort(GLuint width, GLuint height, GLfloat near, GLfloat far)
    {
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::Vector<float, 4>), sizeof(GLuint), &width);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::Vector<float, 4>) + sizeof(GLuint), sizeof(GLuint), &height);

      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::Vector<float, 4>) + 2 * sizeof(GLuint), sizeof(GLfloat), &near);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::Vector<float, 4>) + 2 * sizeof(GLuint) + sizeof(GLfloat), sizeof(GLfloat), &far);
    }

    void RenderManager::render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition)
    {
      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
      util::Vector<float, 4> eyeVec = util::Vector<float, 4>(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

      m_cameraParameterUBO.setData(0, sizeof(util::Matrix<float, 4>), &viewMatrix[0][0]);
      m_cameraParameterUBO.setData(1 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &projectionMatrix[0][0]);
      m_cameraParameterUBO.setData(2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &viewProjectionMatrix[0][0]);
      m_cameraParameterUBO.setData(3 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &viewProjectionMatrix.invert()[0][0]);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>), sizeof(util::Vector<float, 4>), &eyeVec[0]);
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

        m_gBuffer.setGBuffer();

        m_geometryRasterizer.rasterizeGeometry();

        m_gBuffer.unsetGBuffer();

        if(m_wireframe)
        {
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        m_lightRenderer.updateBuffer();

        //for(unsigned int i = 0; i < ; i++)
        //{
        //  m_lightRenderer.setShadowMap();
        //  m_geometryRasterizer.generateShadowMap();
        //  m_lightRenderer.unsetShadowMap();
        //}

        m_lightRenderer.render(m_gBuffer.getDepthTexture(), m_gBuffer.getNormalTexture(), m_gBuffer.getMaterialTexture());

        m_gBuffer.setGBuffer();

        m_billboardRenderer.render();

        if(m_skyboxRendering)
        {
          m_skyboxRenderer.render();
        }

        m_gBuffer.unsetGBuffer();
      }

      //m_fullscreenRenderQuad.setReadTextures(1, m_lightRenderer.getLightTexture());
      //db::RenderShader *shader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_offscreenBufferShaderHandle);

      m_fullscreenRenderQuad.setReadTextures(2, m_gBuffer.getColorTexture(), m_lightRenderer.getLightTexture());
      db::RenderShader *shader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_combineShaderHandle);

      shader->useShader();
      m_fullscreenRenderQuad.render();
      shader->useNoShader();

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
