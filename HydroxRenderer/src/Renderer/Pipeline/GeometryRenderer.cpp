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
      m_globalOccluderNumber(0),
      m_globalTriangleNumber(0),
      m_globalVertexNumber(0)
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
      m_noLightingRenderpass.initialize(m_singletonManager, sh::ShaderContainer::RENDERNOLIGHTING, samplerObjects);
      m_reflectiveShadowMapRenderpass.initialize(m_singletonManager, sh::ShaderContainer::REFLECTIVESHADOW, samplerObjects);
      m_shadowMapRenderpass.initialize(m_singletonManager, sh::ShaderContainer::SHADOW);
      m_indirectBackprojetionRenderpass.initialize(m_singletonManager, sh::ShaderContainer::INDIRECTSHADOWMAPBACKPROJECTION);

      registerRenderComponentSlots(m_singletonManager->getService<util::EventManager>());
    }

    void GeometryRenderer::addRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry)
    {
      db::Material *material = m_singletonManager->getService<db::MaterialManager>()->getObject(geometry->getMaterialHandle());

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
      m_noLightingRenderpass.insertGeometry(m_renderContainer.back());
      m_geometryRenderpass.insertGeometry(m_renderContainer.back());
      m_reflectiveShadowMapRenderpass.insertGeometry(m_renderContainer.back());
      m_shadowMapRenderpass.insertGeometry(m_renderContainer.back());
      m_indirectBackprojetionRenderpass.insertGeometry(m_renderContainer.back());
    }

    void GeometryRenderer::removeRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry)
    {
      for(unsigned int i = 0; i < m_renderContainer.size(); i++)
      {
        if(m_renderContainer[i]->removeGeometry(geometry))
        {
          if(m_renderContainer[i]->isEmpty())
          {
            m_frustumCullingRenderpass.removeGeometry(m_renderContainer[i]);
            m_noLightingRenderpass.removeGeometry(m_renderContainer[i]);
            m_geometryRenderpass.removeGeometry(m_renderContainer[i]);
            m_noLightingRenderpass.removeGeometry(m_renderContainer[i]);
            m_reflectiveShadowMapRenderpass.removeGeometry(m_renderContainer[i]);
            m_shadowMapRenderpass.removeGeometry(m_renderContainer[i]);
            m_indirectBackprojetionRenderpass.removeGeometry(m_renderContainer[i]);

            m_renderContainer.erase(m_renderContainer.begin() + i);

            return;
          }
        }
      }
    }

    void GeometryRenderer::updateBuffer()
    {
      m_globalOccluderNumber = 0;
      m_globalTriangleNumber = 0;
      m_globalVertexNumber = 0;

      for(unsigned int i = 0; i < m_renderContainer.size(); i++)
      {
        m_renderContainer[i]->update();
        m_globalOccluderNumber += m_renderContainer[i]->getPerInstanceOccluderNumber();
        m_globalTriangleNumber += m_renderContainer[i]->getPerInstanceTriangleNumber();
        m_globalVertexNumber += m_renderContainer[i]->getPerInstanceVertexNumber();
      }
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

    void GeometryRenderer::generateIndirectBackprojectionMap(unsigned int lightIndex)
    {
      m_indirectBackprojetionRenderpass.setLightIndex(lightIndex);
      m_indirectBackprojetionRenderpass.drawRenderpass();
    }
  
    void GeometryRenderer::rasterizeGeometry()
    {
      m_geometryRenderpass.drawRenderpass();
    }

    void GeometryRenderer::rasterizeNoLightingGeometry()
    {
      m_noLightingRenderpass.drawRenderpass();
    }

    unsigned int GeometryRenderer::getGlobalOccluderNumber() const
    {
      return m_globalOccluderNumber;
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