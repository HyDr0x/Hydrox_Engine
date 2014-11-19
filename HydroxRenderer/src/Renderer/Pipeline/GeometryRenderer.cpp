#include "Renderer/Pipeline/GeometryRenderer.h"

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"
#include "Renderer/Traverser/InsertShadowGeometryTraverser.h"
#include "Renderer/Traverser/InsertReflectiveShadowGeometryTraverser.h"
#include "Renderer/Traverser/RemoveGeometryTraverser.h"
#include "Renderer/Traverser/RemoveShadowGeometryTraverser.h"
#include "Renderer/Traverser/RemoveReflectiveShadowGeometryTraverser.h"
#include "Renderer/Traverser/RenderGeometryTraverser.h"
#include "Renderer/Traverser/RenderShadowGeometryTraverser.h"
#include "Renderer/Traverser/FrustumCullingTraverser.h"
#include "Renderer/Traverser/UpdateTraverser.h"

#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

#include "Renderer/Pipeline/RenderShaderContainer.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    GeometryRenderer::GeometryRenderer() : 
      m_renderRootNode(nullptr), 
      m_renderShadowRootNode(nullptr), 
      m_renderReflectiveShadowRootNode(nullptr),
      m_globalCacheNumber(0)
    {
    }

    GeometryRenderer::~GeometryRenderer()
    {
      delete m_renderRootNode;
      delete m_renderShadowRootNode;
      delete m_renderReflectiveShadowRootNode;
    }

    void GeometryRenderer::initialize(util::SingletonManager *singletonManager)
    {
      m_singletonManager = singletonManager;

      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glDepthFunc(GL_LESS);
      glClearDepth(1.0f);

      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

      m_renderRootNode = new GroupNode();
      m_renderShadowRootNode = new GroupNode();
      m_renderReflectiveShadowRootNode = new GroupNode();

      m_container = singletonManager->getService<RenderShaderContainer>();

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());
    }

    void GeometryRenderer::addRenderComponent(const xBar::IGeometryContainer& geometry)
    {
      m_globalCacheNumber += m_singletonManager->getService<db::ModelManager>()->getObject(geometry.getMeshHandle())->getCacheCount();

      InsertGeometryTraverser insertTraverser(geometry, m_singletonManager);
      insertTraverser.doTraverse(m_renderRootNode);

      util::SharedPointer<IRenderGroup> createdRenderGroup = insertTraverser.getCreatedRenderNode();

      if(createdRenderGroup)
      {
        InsertShadowGeometryTraverser insertShadowTraverser(createdRenderGroup, geometry, m_singletonManager,
          m_container->staticShadowMapGenerationShaderHandle,
          m_container->skinnedShadowMapGenerationShaderHandle);

        insertShadowTraverser.doTraverse(m_renderShadowRootNode);

        InsertReflectiveShadowGeometryTraverser insertReflectiveShadowTraverser(createdRenderGroup, geometry, m_singletonManager, m_container);

        insertReflectiveShadowTraverser.doTraverse(m_renderReflectiveShadowRootNode);
      }
    }

    void GeometryRenderer::removeRenderComponent(const xBar::IGeometryContainer& geometry)
    {
      m_globalCacheNumber -= m_singletonManager->getService<db::ModelManager>()->getObject(geometry.getMeshHandle())->getCacheCount();

      RemoveGeometryTraverser removeTraverser(m_singletonManager, geometry);
      removeTraverser.doTraverse(m_renderRootNode);

      RemoveShadowGeometryTraverser removeShadowTraverser(m_singletonManager, geometry, 
        m_container->staticShadowMapGenerationShaderHandle,
        m_container->skinnedShadowMapGenerationShaderHandle);

      removeShadowTraverser.doTraverse(m_renderShadowRootNode);

      RemoveReflectiveShadowGeometryTraverser removeReflectiveShadowTraverser(m_singletonManager, geometry, m_container);

      removeReflectiveShadowTraverser.doTraverse(m_renderReflectiveShadowRootNode);
    }

    void GeometryRenderer::updateBuffer()
    {
      UpdateTraverser updateTraverser;
      updateTraverser.doTraverse(m_renderRootNode);
      updateTraverser.doTraverse(m_renderShadowRootNode);
      updateTraverser.doTraverse(m_renderReflectiveShadowRootNode);
    }

    void GeometryRenderer::frustumCulling(int shadowMapIndex, RenderPass pass)
    {
      db::ComputeShader *frustumCullingShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_container->frustumCullingShaderHandle);
      frustumCullingShader->useShader();

      db::ComputeShader::setUniform(1, GL_INT, &shadowMapIndex);
      FrustumCullingTraverser frustumCullingTraverser;
      
      switch(pass)
      {
        case SHADOWPASS:
          frustumCullingTraverser.doTraverse(m_renderShadowRootNode);
          break;
        case REFLECTIVESHADOWPASS:
          frustumCullingTraverser.doTraverse(m_renderReflectiveShadowRootNode);
          break;
        case VIEWPASS:
          frustumCullingTraverser.doTraverse(m_renderRootNode);
          break;
      }

      frustumCullingShader->useNoShader();
    }

    void GeometryRenderer::generateShadowMap(int shadowMapIndex)
    {
      RenderShadowGeometryTraverser renderShadowTraverser(m_singletonManager, shadowMapIndex);
      renderShadowTraverser.doTraverse(m_renderShadowRootNode);
    }

    void GeometryRenderer::generateReflectiveShadowMap(int shadowMapIndex)
    {
      RenderShadowGeometryTraverser renderShadowTraverser(m_singletonManager, shadowMapIndex);
      renderShadowTraverser.doTraverse(m_renderReflectiveShadowRootNode);
    }

    void GeometryRenderer::rasterizeGeometry()
    {  
      RenderGeometryTraverser renderTraverser(m_singletonManager);
      renderTraverser.doTraverse(m_renderRootNode);

      //std::vector<util::Cache> tmpCaches(m_globalCacheNumber);
      //m_globalCacheBuffer.getData(0, m_globalCacheNumber * sizeof(util::Cache), &tmpCaches[0]);

      //std::vector<GLuint> tmpZBuffer(m_globalCacheNumber);
      //m_zBuffer.getData(0, m_globalCacheNumber * sizeof(GLuint), &tmpZBuffer[0]);

      //RenderOptions *renderOptions = m_singletonManager->getService<RenderOptions>();
      //std::vector<util::vec4ui> tmpFrameBuffer(renderOptions->width * renderOptions->height * 6);
      //m_frameCacheIndexBuffer.getData(0, renderOptions->width * renderOptions->height * 6 * sizeof(util::vec4ui), &tmpFrameBuffer[0]);
    }

    unsigned int GeometryRenderer::getGlobalCacheNumber() const
    {
      return m_globalCacheNumber;
    }

    void GeometryRenderer::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void(*)(const xBar::IGeometryContainer &geometry)>(util::EventManager::OnAddGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void(*)(const xBar::IGeometryContainer &geometry), void (GeometryRenderer::*)(const xBar::IGeometryContainer &geometry)>(this, &GeometryRenderer::addRenderComponent, util::EventManager::OnAddGeometryNode);
      
      eventManager->addNewSignal<void(*)(const xBar::IGeometryContainer& geometry)>(util::EventManager::OnRemoveGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void(*)(const xBar::IGeometryContainer& geometry), void (GeometryRenderer::*)(const xBar::IGeometryContainer& geometry)>(this, &GeometryRenderer::removeRenderComponent, util::EventManager::OnRemoveGeometryNode);
    }
  }
}