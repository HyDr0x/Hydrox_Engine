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
    RenderManager::RenderManager() : m_skyboxRendering(false), m_wireframe(false), m_showVirtualAreaLights(false), m_showCaches(false), m_debugTexture(0)
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

    void RenderManager::showVirtualAreaLights(bool showVirtualAreaLights)
    {
      if(m_showVirtualAreaLights && !showVirtualAreaLights)
      {
        removeVirtualAreaLights();
      }

      m_showVirtualAreaLights = showVirtualAreaLights;
    }

    void RenderManager::showCaches(bool showCaches)
    {
      if(m_showCaches && !showCaches)
      {
        removeCaches();
      }

      m_showCaches = showCaches;
    }

    void RenderManager::initialize(util::SingletonManager *singletonManager)
    {
      m_singletonManager = singletonManager;

      m_options = m_singletonManager->getService<RenderOptions>();

      m_gBuffer.initialize(m_singletonManager);
      m_geometryRasterizer.initialize(m_singletonManager, m_gBuffer.getNormalTexture(), m_gBuffer.getMaterialTexture());
      m_billboardRenderer.initialize(m_singletonManager);
      m_spriteRenderer.initialize(m_singletonManager);
      m_stringRenderer.initialize(m_singletonManager);
      m_lightRenderer.initialize(m_singletonManager);
      m_indirectLightRenderer.initialize(m_singletonManager);
      m_particleRenderer.initialize(m_singletonManager);
      m_finalCompositing.initialize(m_singletonManager);
      m_tonemapper.initialize(m_singletonManager);

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 4 + sizeof(util::vec4f) + sizeof(GLfloat) * 2 + sizeof(GLuint) * 2, GL_DYNAMIC_DRAW);

      std::vector<util::vec3f> positions, normals;
      std::vector<db::Mesh::indexType> indices;
      std::vector<db::Mesh::VertexDeclarationElements> vertexDeclarationElements;
      vertexDeclarationElements.push_back(db::Mesh::MODEL_POSITION);
      vertexDeclarationElements.push_back(db::Mesh::MODEL_NORMAL);

      db::Material::MaterialData sphereMaterialData(0.5f, 0.5f, 0.0f, 1.0f, util::vec4f(0.0f, 0.0f, 1.0f, 1.0f));
      db::Material debugSphereMaterial(sphereMaterialData, std::vector<std::vector<util::ResourceHandle>>(db::Material::TEXTURETYPENUM), std::vector<std::vector<uint64_t>>(db::Material::TEXTURETYPENUM), false, true);
      m_debugSphereMaterialHandle = m_singletonManager->getService<db::MaterialManager>()->addObject(debugSphereMaterial);

      util::SphereGenerator::generateSphere(0.05f, positions, indices, normals, 5);
      db::Mesh debugProxyLightMesh(GL_TRIANGLES, positions.size(), vertexDeclarationElements, indices);
      debugProxyLightMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_POSITION, 0, positions.size(), reinterpret_cast<const GLubyte*>(&positions[0]));
      debugProxyLightMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_NORMAL, 0, normals.size(), reinterpret_cast<const GLubyte*>(&normals[0]));

      m_debugProxyLightMeshHandle = m_singletonManager->getService<db::ModelManager>()->addObject(debugProxyLightMesh);

      positions.clear();
      indices.clear();
      normals.clear();

      db::Material::MaterialData discMaterialData(0.5f, 0.5f, 0.0f, 1.0f, util::vec4f(1.0f, 0.0f, 1.0f, 1.0f));
      db::Material debugDiscMaterial(discMaterialData, std::vector<std::vector<util::ResourceHandle>>(db::Material::TEXTURETYPENUM), std::vector<std::vector<uint64_t>>(db::Material::TEXTURETYPENUM), false, true);
      m_debugDiscMaterialHandle = m_singletonManager->getService<db::MaterialManager>()->addObject(debugDiscMaterial);

      util::DiscGenerator::generateDisc(0.1f, util::vec3f(0, 1, 0), positions, indices, normals, 20);
      db::Mesh debugCacheMesh(GL_TRIANGLES, positions.size(), vertexDeclarationElements, indices);
      debugCacheMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_POSITION, 0, positions.size(), reinterpret_cast<const GLubyte*>(&positions[0]));
      debugCacheMesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_NORMAL, 0, normals.size(), reinterpret_cast<const GLubyte*>(&normals[0]));

      m_debugCacheMeshHandle = m_singletonManager->getService<db::ModelManager>()->addObject(debugCacheMesh);
    }

    void RenderManager::setViewPort(GLuint width, GLuint height, GLfloat zNear, GLfloat zFar)
    {
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f), sizeof(GLuint), &width);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + sizeof(GLuint), sizeof(GLuint), &height);

      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + 2 * sizeof(GLuint), sizeof(GLfloat), &zNear);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>) + sizeof(util::vec4f) + 2 * sizeof(GLuint) + sizeof(GLfloat), sizeof(GLfloat), &zFar);
    }

    void RenderManager::render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::vec3f& cameraPosition)
    {
      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;

      m_cameraParameterUBO.setData(0, sizeof(util::Matrix<float, 4>), &viewMatrix[0][0]);
      m_cameraParameterUBO.setData(1 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &projectionMatrix[0][0]);
      m_cameraParameterUBO.setData(2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &viewProjectionMatrix[0][0]);
      m_cameraParameterUBO.setData(3 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &viewProjectionMatrix.invert()[0][0]);
      m_cameraParameterUBO.setData(4 * sizeof(util::Matrix<float, 4>), sizeof(util::vec4f), &cameraPosition[0]);
      m_cameraParameterUBO.uploadData();
      m_cameraParameterUBO.bindBuffer(0);

      bool globalIllumination = m_lightRenderer.getReflectiveShadowLightNumber() > 0 && m_geometryRasterizer.getGlobalCacheNumber() > 0 && m_options->globalIllumination;
      
      {//everything in here should be packed in an render pass interface and the blocks like gbuffer etc. in a technique interface which gets passed to a render pass 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_gBuffer.clear();
        m_lightRenderer.clear();
        
        m_geometryRasterizer.updateBuffer();
        m_indirectLightRenderer.updateBuffer(m_geometryRasterizer.getGlobalCacheNumber(), m_geometryRasterizer.getProxyLightTextureResolution());
        m_lightRenderer.updateBuffer();
        
        if(m_wireframe)
        {
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        //m_geometryRasterizer.frustumCulling(-1, VIEWPASS);
        
        m_gBuffer.setGBuffer();
        m_geometryRasterizer.rasterizeGeometry();
        m_gBuffer.unsetGBuffer();
        
        if(globalIllumination)//create global cache map and z buffer
        {
          glDepthMask(GL_FALSE);
          glDepthFunc(GL_LEQUAL);
          m_indirectLightRenderer.setBuffer(m_gBuffer.getDepthTexture());
          m_geometryRasterizer.rasterizeIndexGeometry();
          m_indirectLightRenderer.unsetBuffer();
          glDepthFunc(GL_LESS);
          glDepthMask(GL_TRUE);
        }
        
        {
          glEnable(GL_POLYGON_OFFSET_FILL);
          glPolygonOffset(1.1f, 4.0f);
          glViewport(0, 0, m_options->shadowMapWidth, m_options->shadowMapWidth);
          for(unsigned int i = 0; i < m_lightRenderer.getShadowLightNumber(); i++)
          {
            m_lightRenderer.setShadowMap(4, i);
            //m_geometryRasterizer.frustumCulling(i, SHADOWPASS);
            m_geometryRasterizer.generateShadowMap(i);
            m_lightRenderer.unsetShadowMap(4);
          }
        }

        {
          glViewport(0, 0, m_options->reflectiveShadowMapWidth, m_options->reflectiveShadowMapWidth);
          for(unsigned int i = 0; i < m_lightRenderer.getReflectiveShadowLightNumber(); i++)
          {
            m_lightRenderer.setReflectiveShadowMap(4, i);
            //m_geometryRasterizer.frustumCulling(i, REFLECTIVESHADOWPASS);
            m_geometryRasterizer.generateReflectiveShadowMap(i);
            m_lightRenderer.unsetReflectiveShadowMap(4);
          }
          glViewport(0, 0, m_options->width, m_options->height);
          glDisable(GL_POLYGON_OFFSET_FILL);
        }

        if(m_wireframe)
        {
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        m_lightRenderer.render(m_gBuffer.getDepthTexture(), m_gBuffer.getNormalTexture(), m_gBuffer.getMaterialTexture());
        
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

        if(globalIllumination)
        {
          m_indirectLightRenderer.calculateIndirectLight(
            m_lightRenderer.getReflectiveShadowPosMaps(),
            m_lightRenderer.getReflectiveShadowNormalMaps(),
            m_lightRenderer.getReflectiveShadowLuminousFluxMaps(),
            m_lightRenderer.getReflectiveShadowLights());

          m_indirectLightRenderer.setCacheAndProxyLights();//create indirect light map
          m_geometryRasterizer.generateIndirectLightMap();
          m_indirectLightRenderer.unsetCacheAndProxyLights();
        }

        if(m_showVirtualAreaLights && globalIllumination)
        {
          renderVirtualAreaLights();
        }

        if(m_showCaches && globalIllumination)
        {
          renderCaches();
        }

        m_gBuffer.setDebugGBuffer();
        m_geometryRasterizer.rasterizeDebugGeometry();
        m_gBuffer.unsetDebugGBuffer();
      }
      
      switch(m_debugTexture)
      {
      case 1:
        m_finalCompositing.renderDebugOutput(m_gBuffer.getColorTexture());
        break;
      case 2:
        m_finalCompositing.renderDebugOutput(m_gBuffer.getNormalTexture());
        break;
      case 3:
        m_finalCompositing.renderDebugOutput(m_gBuffer.getMaterialTexture());
        break;
      case 4:
        m_finalCompositing.renderDebugOutput(m_gBuffer.getDepthTexture());
        break;
      case 5:
        m_finalCompositing.renderDebugOutput(m_lightRenderer.getLightTexture());
        break;
      case 6:
        m_finalCompositing.renderDebugOutput(m_lightRenderer.getReflectiveShadowPosMaps()->convertToTexture2D(0));
        break;
      case 7:
        m_finalCompositing.renderDebugOutput(m_lightRenderer.getReflectiveShadowNormalMaps()->convertToTexture2D(0));
        break;
      case 8:
        m_finalCompositing.renderDebugOutput(m_lightRenderer.getReflectiveShadowLuminousFluxMaps()->convertToTexture2D(0));
        break;
      case 9:
        m_finalCompositing.renderDebugOutput(m_indirectLightRenderer.getIndirectLightMap());
        break;
      case 10:
        m_finalCompositing.renderDebugOutput(m_indirectLightRenderer.getSamplingDebugMap());
        break;
      case 11:
        m_finalCompositing.composeImage(m_gBuffer.getColorTexture(), m_lightRenderer.getLightTexture(), m_indirectLightRenderer.getIndirectLightMap());
        m_tonemapper.doToneMapping(m_finalCompositing.getCombinedTexture());
        break;
      case 0:
      default:
        m_finalCompositing.composeImage(m_gBuffer.getColorTexture(), m_lightRenderer.getLightTexture(), m_indirectLightRenderer.getIndirectLightMap());
        m_finalCompositing.renderDebugOutput(m_finalCompositing.getCombinedTexture());
      }
      
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

    void RenderManager::renderVirtualAreaLights()
    {
      if(m_geometryRasterizer.getGlobalCacheNumber() != m_trfProxyLightMatrices.size())
      {
        unsigned int diff = abs(int(m_trfProxyLightMatrices.size()) - int(m_geometryRasterizer.getGlobalCacheNumber()));

        if(m_geometryRasterizer.getGlobalCacheNumber() < m_trfProxyLightMatrices.size())
        {
          unsigned int oldSize = m_trfProxyLightMatrices.size() - diff;
          for(unsigned int i = 0; i < diff; i++)
          {
            util::SharedPointer<const xBar::StaticGeometryContainer> geomContainer(new const xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE), m_trfProxyLightMatrices[oldSize + i].get(), m_debugSphereMaterialHandle, m_debugProxyLightMeshHandle));
            removeRenderComponent(geomContainer);
          }

          m_trfProxyLightMatrices.resize(m_geometryRasterizer.getGlobalCacheNumber());
        }
        else
        {
          unsigned int oldSize = m_trfProxyLightMatrices.size();
          m_trfProxyLightMatrices.resize(m_geometryRasterizer.getGlobalCacheNumber());

          for(unsigned int i = 0; i < diff; i++)
          {
            m_trfProxyLightMatrices[oldSize + i] = util::SharedPointer<util::Matrix<float, 4>>(new util::Matrix<float, 4>(util::Matrix<float, 4>::identity()));
            util::SharedPointer<const xBar::StaticGeometryContainer> geomContainer(new const xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE), m_trfProxyLightMatrices[oldSize + i].get(), m_debugSphereMaterialHandle, m_debugProxyLightMeshHandle));
            addRenderComponent(geomContainer);
          }
        }
      }

      unsigned int cacheResolution = m_geometryRasterizer.getProxyLightTextureResolution();
      
      std::vector<util::vec4f> lightPositions(2 * cacheResolution * cacheResolution);
      std::vector<util::vec4f> lightLuminousFlux(2 * cacheResolution * cacheResolution);
      std::vector<util::Vector<GLubyte, 4>> zBuffer(cacheResolution * cacheResolution);
      
      m_indirectLightRenderer.getIndirectPositionMap()->getTextureData(&lightPositions[0]);
      m_indirectLightRenderer.getIndirectLuminousFluxMap()->getTextureData(&lightLuminousFlux[0]);
      m_indirectLightRenderer.getZBuffer()->getTextureData(GL_RGBA, GL_UNSIGNED_BYTE, &zBuffer[0]);

      unsigned int validLightCounter = 0;
      for(unsigned int i = 0; i < cacheResolution * cacheResolution; i++)
      {
        if(zBuffer[i][0])
        {
          *m_trfProxyLightMatrices[validLightCounter] = util::math::createTransformationMatrix(util::vec3f(lightPositions[2 * i][0], lightPositions[2 * i][1], lightPositions[2 * i][2]), 1.0f, util::math::createRotAxisQuaternion(0.0f, util::vec3f(0.0f, 1.0f, 0.0f)));
          validLightCounter++;
        }
      }

      for(unsigned int i = validLightCounter; i < m_trfProxyLightMatrices.size(); i++)
      {
        *m_trfProxyLightMatrices[i] = util::Matrix<float, 4>::identity();
      }
    }

    void RenderManager::removeVirtualAreaLights()
    {
      for(unsigned int i = 0; i < m_trfProxyLightMatrices.size(); i++)
      {
        removeRenderComponent(util::SharedPointer<const xBar::StaticGeometryContainer>(new xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE), m_trfProxyLightMatrices[i].get(), m_debugSphereMaterialHandle, m_debugProxyLightMeshHandle)));
      }

      m_trfProxyLightMatrices.clear();
    }

    util::vec2f encodeNormal2(util::vec3f normal)
    {
      return normal[2] < 1.0f - 0.00001 ? (normal[2] * 0.5f + 0.5f) * (util::vec2f(normal[0], normal[1])).normalize() : util::vec2f(1.0f, 0.0f);
    }

    util::vec3f decodeNormal2(util::vec2f normal)
    {
      float z = normal.length() * 2.0f - 1.0f;

      util::vec2f tmpErg = z > -1.0f + 0.00001f ? sqrt((1.0f - z * z) / (util::vec2f::dot(normal, normal))) * normal : util::vec2f(0.0f, 0.0f);
      return util::vec3f(tmpErg[0], tmpErg[1], z);
    }

    void RenderManager::renderCaches()
    {
      if(m_geometryRasterizer.getGlobalCacheNumber() != m_trfCacheMatrices.size())
      {
        unsigned int diff = abs(int(m_trfCacheMatrices.size()) - int(m_geometryRasterizer.getGlobalCacheNumber()));

        if(m_geometryRasterizer.getGlobalCacheNumber() < m_trfCacheMatrices.size())
        {
          unsigned int oldSize = m_trfCacheMatrices.size() - diff;
          for(unsigned int i = 0; i < diff; i++)
          {
            util::SharedPointer<const xBar::StaticGeometryContainer> geomContainer(new const xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE), m_trfCacheMatrices[oldSize + i].get(), m_debugDiscMaterialHandle, m_debugCacheMeshHandle));
            removeRenderComponent(geomContainer);
          }

          m_trfCacheMatrices.resize(m_geometryRasterizer.getGlobalCacheNumber());
        }
        else
        {
          unsigned int oldSize = m_trfCacheMatrices.size();
          m_trfCacheMatrices.resize(m_geometryRasterizer.getGlobalCacheNumber());

          for(unsigned int i = 0; i < diff; i++)
          {
            m_trfCacheMatrices[oldSize + i] = util::SharedPointer<util::Matrix<float, 4>>(new util::Matrix<float, 4>(util::Matrix<float, 4>::identity()));
            util::SharedPointer<const xBar::StaticGeometryContainer> geomContainer(new const xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE), m_trfCacheMatrices[oldSize + i].get(), m_debugDiscMaterialHandle, m_debugCacheMeshHandle));
            addRenderComponent(geomContainer);
          }
        }
      }
      
      unsigned int cacheNumber = m_geometryRasterizer.getGlobalCacheNumber();
      unsigned int cacheTextureResolution = m_geometryRasterizer.getProxyLightTextureResolution();

      std::vector<util::vec4f> cachePositions(cacheNumber);
      std::vector<util::vec4f> cacheNormals(cacheNumber);
      std::vector<util::Vector<GLubyte, 4>> zBuffer(cacheTextureResolution * cacheTextureResolution);

      m_indirectLightRenderer.getGlobalCachePositionMap().getData(0, sizeof(util::vec4f) * cacheNumber, &cachePositions[0]);
      m_indirectLightRenderer.getGlobalCacheNormalMap().getData(0, sizeof(util::vec4f) * cacheNumber, &cacheNormals[0]);
      m_indirectLightRenderer.getZBuffer()->getTextureData(GL_RGBA, GL_UNSIGNED_BYTE, &zBuffer[0]);
      
      unsigned int validLightCounter = 0;
      for(unsigned int i = 0; i < cachePositions.size(); i++)
      {
        if(zBuffer[i][0])
        {
          util::vec3f normal = decodeNormal2(util::vec2f(cacheNormals[i][0], cacheNormals[i][1]));
          util::vec3f rotationAxis = util::math::cross(normal, util::vec3f(0, 1, 0)).normalize();
          float angle = acosf(util::math::clamp(util::vec3f::dot(util::vec3f(0, 1, 0), normal), -1.0f, 1.0f));

          cachePositions[i] += normal * 0.01f;

          *m_trfCacheMatrices[validLightCounter] = util::math::createTransformationMatrix(util::vec3f(cachePositions[i][0], cachePositions[i][1], cachePositions[i][2]), 1.0f, util::math::createRotAxisQuaternion<float>(angle, rotationAxis));
          validLightCounter++;
        }
      }

      for(unsigned int i = validLightCounter; i < m_trfCacheMatrices.size(); i++)
      {
        *m_trfCacheMatrices[i] = util::Matrix<float, 4>::identity();
      }
    }

    void RenderManager::removeCaches()
    {
      for(unsigned int i = 0; i < m_trfCacheMatrices.size(); i++)
      {
        removeRenderComponent(util::SharedPointer<const xBar::StaticGeometryContainer>(new xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE), m_trfCacheMatrices[i].get(), m_debugDiscMaterialHandle, m_debugCacheMeshHandle)));
      }

      m_trfCacheMatrices.clear();
    }
  }
}
