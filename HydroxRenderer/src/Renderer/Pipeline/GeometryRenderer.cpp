#include "Renderer/Pipeline/GeometryRenderer.h"

#include <Utilities/Timer/Timer.h>

#include <Shader/ShaderContainer.h>

#include <XBar/IGeometryContainer.h>

#include "Renderer/Renderdata/RenderDataFactory.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    GeometryRenderer::GeometryRenderer() :
      m_globalCacheNumber(0)
    {
    }

    GeometryRenderer::~GeometryRenderer()
    {
    }

    void GeometryRenderer::initialize(util::SingletonManager *singletonManager, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap)
    {
      m_singletonManager = singletonManager;

      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glDepthFunc(GL_LESS);
      glClearDepth(1.0f);

      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

      m_container = m_singletonManager->getService<sh::ShaderContainer>();

      std::vector<util::SharedPointer<SamplerObject>> samplerObjects(db::Material::TEXTURETYPENUM);
      samplerObjects[db::Material::DIFFUSETEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
      samplerObjects[db::Material::NORMALTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
      samplerObjects[db::Material::METALNESSTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
      samplerObjects[db::Material::ROUGHNESSTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR));

      m_frustumCullingRenderpass.initialize(m_singletonManager, sh::ShaderContainer::FRUSTUMCULLING);
      m_geometryRenderpass.initialize(m_singletonManager, sh::ShaderContainer::GBUFFER, samplerObjects);
      m_debugRenderpass.initialize(m_singletonManager, sh::ShaderContainer::RENDERDEBUG, samplerObjects);
      m_indexRenderpass.initialize(m_singletonManager, sh::ShaderContainer::TRIANGLEINDEX);
      m_indirectLightEdgeVertexExtractionRenderpass.initialize(m_singletonManager, sh::ShaderContainer::INDIRECTLIGHTSCREENVERTEXEXTRACTION, materialMap);
      m_indirectLightingRenderpass.initialize(m_singletonManager, sh::ShaderContainer::INDIRECTLIGHTINTERPOLATION, normalMap, materialMap);
      m_reflectiveShadowMapRenderpass.initialize(m_singletonManager, sh::ShaderContainer::REFLECTIVESHADOW, samplerObjects);
      m_shadowMapRenderpass.initialize(m_singletonManager, sh::ShaderContainer::SHADOW);
      m_indirectBackprojetionRenderpass.initialize(m_singletonManager, sh::ShaderContainer::INDIRECTSHADOWMAPBACKPROJECTION);

      registerRenderComponentSlots(m_singletonManager->getService<util::EventManager>());
    }

    void GeometryRenderer::addRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry)
    {
      db::Material *material = m_singletonManager->getService<db::MaterialManager>()->getObject(geometry->getMaterialHandle());

      if(!material->getDebug())
      {
        for(unsigned int i = 0; i < m_renderContainer.size(); i++)
        {
          if(m_renderContainer[i]->insertGeometry(geometry))
          {
            return;
          }
        }

        m_renderContainer.push_back(RenderDataFactory::createRenderDataContainer(geometry, m_singletonManager));

        m_renderContainer.back()->insertGeometry(geometry);

        m_frustumCullingRenderpass.insertGeometry(m_renderContainer.back());
        m_geometryRenderpass.insertGeometry(m_renderContainer.back());
        m_indexRenderpass.insertGeometry(m_renderContainer.back());
        m_indirectLightEdgeVertexExtractionRenderpass.insertGeometry(m_renderContainer.back());
        m_indirectLightingRenderpass.insertGeometry(m_renderContainer.back());
        m_reflectiveShadowMapRenderpass.insertGeometry(m_renderContainer.back());
        m_shadowMapRenderpass.insertGeometry(m_renderContainer.back());
        m_indirectBackprojetionRenderpass.insertGeometry(m_renderContainer.back());
      }
      else
      {
        for(unsigned int i = 0; i < m_debugRenderContainer.size(); i++)
        {
          if(m_debugRenderContainer[i]->insertGeometry(geometry))
          {
            return;
          }
        }

        m_debugRenderContainer.push_back(RenderDataFactory::createRenderDataContainer(geometry, m_singletonManager));

        m_debugRenderContainer.back()->insertGeometry(geometry);

        m_debugRenderpass.insertGeometry(m_debugRenderContainer.back());
      }
    }

    void GeometryRenderer::removeRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry)
    {
      if(!m_singletonManager->getService<db::MaterialManager>()->getObject(geometry->getMaterialHandle())->getDebug())
      {
        for(unsigned int i = 0; i < m_renderContainer.size(); i++)
        {
          if(m_renderContainer[i]->removeGeometry(geometry))
          {
            if(m_renderContainer[i]->isEmpty())
            {
              m_frustumCullingRenderpass.removeGeometry(m_renderContainer[i]);
              m_geometryRenderpass.removeGeometry(m_renderContainer[i]);
              m_debugRenderpass.removeGeometry(m_renderContainer[i]);
              m_indexRenderpass.removeGeometry(m_renderContainer[i]);
              m_indirectLightEdgeVertexExtractionRenderpass.removeGeometry(m_renderContainer[i]);
              m_indirectLightingRenderpass.removeGeometry(m_renderContainer[i]);
              m_reflectiveShadowMapRenderpass.removeGeometry(m_renderContainer[i]);
              m_shadowMapRenderpass.removeGeometry(m_renderContainer[i]);
              m_indirectBackprojetionRenderpass.removeGeometry(m_renderContainer[i]);

              m_renderContainer.erase(m_renderContainer.begin() + i);

              return;
            }
          }
        }
      }
      else
      {
        for(unsigned int i = 0; i < m_debugRenderContainer.size(); i++)
        {
          if(m_debugRenderContainer[i]->removeGeometry(geometry))
          {
            if(m_debugRenderContainer[i]->isEmpty())
            {
              m_debugRenderpass.removeGeometry(m_debugRenderContainer[i]);

              m_debugRenderContainer.erase(m_debugRenderContainer.begin() + i);
            }

            return;
          }
        }
      }
    }

    void GeometryRenderer::updateBuffer()
    {
      m_globalCacheNumber = 0;

      for(unsigned int i = 0; i < m_renderContainer.size(); i++)
      {
        m_renderContainer[i]->update();
        m_globalCacheNumber += m_renderContainer[i]->getCacheNumber();
      }

      for(unsigned int i = 0; i < m_debugRenderContainer.size(); i++)
      {
        m_debugRenderContainer[i]->update();
      }

      m_proxyLightTextureResolution = static_cast<unsigned int>(pow(2.0f, ceil(log(sqrt(float(m_globalCacheNumber))) / log(2.0f))));
    }

    void GeometryRenderer::frustumCulling(int cameraIndex, Renderpass pass)
    {
      m_frustumCullingRenderpass.setViewIndex(cameraIndex);
      m_frustumCullingRenderpass.doComputepass();
    }

    void GeometryRenderer::generateShadowMap(unsigned int lightIndex)
    {
      m_shadowMapRenderpass.setLightIndex(lightIndex);
      m_shadowMapRenderpass.drawRenderpass();
    }

    void GeometryRenderer::generateParaboloidShadowMap(unsigned int lightIndex)
    {
      m_paraboloidShadowMappingpass.setLightIndex(lightIndex);
      m_paraboloidShadowMappingpass.drawRenderpass();
    }

    void GeometryRenderer::generateReflectiveShadowMap(unsigned int lightIndex)
    {
      m_reflectiveShadowMapRenderpass.setLightIndex(lightIndex);
      m_reflectiveShadowMapRenderpass.drawRenderpass();
    }

    void GeometryRenderer::generateIndirectLightMap(util::SharedPointer<db::Texture2D> indirectShadowMap)
    {
      m_indirectLightingRenderpass.setProxyLightTextureResolution(m_proxyLightTextureResolution);
      m_indirectLightingRenderpass.setIndirectShadowMap(indirectShadowMap);
      m_indirectLightingRenderpass.drawRenderpass();
    }

    void GeometryRenderer::generateIndirectBackprojectionMap(unsigned int lightIndex)
    {
      m_indirectBackprojetionRenderpass.setLightIndex(lightIndex);
      m_indirectBackprojetionRenderpass.drawRenderpass();
    }

    void GeometryRenderer::generateEdgeVertexMap()
    {
      m_indirectLightEdgeVertexExtractionRenderpass.drawRenderpass();
    }

    void GeometryRenderer::rasterizeIndexGeometry()
    {
      m_indexRenderpass.setProxyLightTextureResolution(m_proxyLightTextureResolution);
      m_indexRenderpass.drawRenderpass();
    }

    void GeometryRenderer::rasterizeGeometry()
    {
      m_geometryRenderpass.drawRenderpass();
    }

    void GeometryRenderer::rasterizeDebugGeometry()
    {
      m_debugRenderpass.drawRenderpass();
    }

    unsigned int GeometryRenderer::getGlobalCacheNumber() const
    {
      return m_globalCacheNumber;
    }

    unsigned int GeometryRenderer::getProxyLightTextureResolution() const
    {
      return m_proxyLightTextureResolution;
    }

    void GeometryRenderer::registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager)
    {
      eventManager->addNewSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnAddGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry), void (GeometryRenderer::*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(this, &GeometryRenderer::addRenderComponent, util::EventManager::OnAddGeometryNode);
      
      eventManager->addNewSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnRemoveGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry), void (GeometryRenderer::*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(this, &GeometryRenderer::removeRenderComponent, util::EventManager::OnRemoveGeometryNode);
    }
  }
}