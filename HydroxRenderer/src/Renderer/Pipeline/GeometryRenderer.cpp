#include "Renderer/Pipeline/GeometryRenderer.h"

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"
#include "Renderer/Traverser/InsertShadowGeometryTraverser.h"
#include "Renderer/Traverser/RemoveGeometryTraverser.h"
#include "Renderer/Traverser/RemoveShadowGeometryTraverser.h"
#include "Renderer/Traverser/RenderGeometryTraverser.h"
#include "Renderer/Traverser/FrustumCullingTraverser.h"
#include "Renderer/Traverser/ShadowMapTraverser.h"
#include "Renderer/Traverser/UpdateTraverser.h"

#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

#include "Renderer/Pipeline/RenderShaderContainer.h"

namespace he
{
  namespace renderer
  {
    GeometryRenderer::GeometryRenderer() : m_renderRootNode(nullptr), m_renderShadowRootNode(nullptr), m_renderReflectiveShadowRootNode(nullptr)
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
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glDepthFunc(GL_LESS);
      glClearDepth(1.0f);

      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

      glPolygonOffset(1.1f, 4.0f);

      m_renderRootNode = new GroupNode();
      m_renderShadowRootNode = new GroupNode();
      m_renderReflectiveShadowRootNode = new GroupNode();

      m_singletonManager = singletonManager;

      m_frustumCullingShaderHandle = singletonManager->getService<RenderShaderContainer>()->frustumCullingShaderHandle;
      m_staticShadowMapGenerationShaderHandle = singletonManager->getService<RenderShaderContainer>()->staticShadowMapGenerationShaderHandle;
      m_skinnedShadowMapGenerationShaderHandle = singletonManager->getService<RenderShaderContainer>()->skinnedShadowMapGenerationShaderHandle;

      m_staticReflectiveShadowMapGenerationShaderHandle = singletonManager->getService<RenderShaderContainer>()->staticReflectiveShadowMapGenerationShaderHandle;
      m_staticNormalReflectiveShadowMapGenerationShaderHandle = singletonManager->getService<RenderShaderContainer>()->staticNormalReflectiveShadowMapGenerationShaderHandle;
      
      m_skinnedReflectiveShadowMapGenerationShaderHandle = singletonManager->getService<RenderShaderContainer>()->skinnedReflectiveShadowMapGenerationShaderHandle;
      m_skinnedNormalReflectiveShadowMapGenerationShaderHandle = singletonManager->getService<RenderShaderContainer>()->skinnedNormalReflectiveShadowMapGenerationShaderHandle;

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());
    }

    void GeometryRenderer::addRenderComponent(const xBar::IGeometryContainer& geometry)
    {
      InsertGeometryTraverser insertTraverser(geometry, m_singletonManager);
      insertTraverser.doTraverse(m_renderRootNode);

      util::SharedPointer<IRenderGroup> createdRenderGroup = insertTraverser.getCreatedRenderNode();

      if(createdRenderGroup)
      {
        InsertShadowGeometryTraverser insertShadowTraverser(createdRenderGroup, geometry, m_singletonManager,
          m_staticShadowMapGenerationShaderHandle,
          m_staticNormalReflectiveShadowMapGenerationShaderHandle,
          m_skinnedShadowMapGenerationShaderHandle,
          m_skinnedNormalReflectiveShadowMapGenerationShaderHandle);

        insertShadowTraverser.doTraverse(m_renderShadowRootNode);

        InsertShadowGeometryTraverser insertReflectiveShadowTraverser(createdRenderGroup, geometry, m_singletonManager,
          m_staticShadowMapGenerationShaderHandle,
          m_staticNormalReflectiveShadowMapGenerationShaderHandle,
          m_skinnedShadowMapGenerationShaderHandle,
          m_skinnedNormalReflectiveShadowMapGenerationShaderHandle);

        insertReflectiveShadowTraverser.doTraverse(m_renderReflectiveShadowRootNode);
      }
    }

    void GeometryRenderer::removeRenderComponent(const xBar::IGeometryContainer& geometry)
    {
      RemoveGeometryTraverser removeTraverser(m_singletonManager, geometry);
      removeTraverser.doTraverse(m_renderRootNode);

      RemoveShadowGeometryTraverser removeShadowTraverser(m_singletonManager, geometry, 
        m_staticShadowMapGenerationShaderHandle,
        m_staticNormalReflectiveShadowMapGenerationShaderHandle,
        m_skinnedShadowMapGenerationShaderHandle,
        m_skinnedNormalReflectiveShadowMapGenerationShaderHandle);

      removeShadowTraverser.doTraverse(m_renderShadowRootNode);

      RemoveShadowGeometryTraverser removeReflectiveShadowTraverser(m_singletonManager, geometry,
        m_staticShadowMapGenerationShaderHandle,
        m_staticNormalReflectiveShadowMapGenerationShaderHandle,
        m_skinnedShadowMapGenerationShaderHandle,
        m_skinnedNormalReflectiveShadowMapGenerationShaderHandle);

      removeReflectiveShadowTraverser.doTraverse(m_renderReflectiveShadowRootNode);
    }

    void GeometryRenderer::updateBuffer()
    {
      UpdateTraverser updateTraverser;
      updateTraverser.doTraverse(m_renderRootNode);
      updateTraverser.doTraverse(m_renderShadowRootNode);
      updateTraverser.doTraverse(m_renderReflectiveShadowRootNode);
    }

    void GeometryRenderer::generateShadowMap(int shadowMapIndex)
    {
      db::ComputeShader *frustumCullingShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_frustumCullingShaderHandle);
      frustumCullingShader->useShader();
      db::ComputeShader::setUniform(1, GL_INT, &shadowMapIndex);
      FrustumCullingTraverser frustumCullingTraverser;
      frustumCullingTraverser.doTraverse(m_renderShadowRootNode);
      frustumCullingShader->useNoShader();

      RenderGeometryTraverser renderTraverser(m_singletonManager, shadowMapIndex);
      renderTraverser.doTraverse(m_renderShadowRootNode);
    }

    void GeometryRenderer::generateReflectiveShadowMap(int shadowMapIndex)
    {
      db::ComputeShader *frustumCullingShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_frustumCullingShaderHandle);
      frustumCullingShader->useShader();
      db::ComputeShader::setUniform(1, GL_INT, &shadowMapIndex);
      FrustumCullingTraverser frustumCullingTraverser;
      frustumCullingTraverser.doTraverse(m_renderReflectiveShadowRootNode);
      frustumCullingShader->useNoShader();

      RenderGeometryTraverser renderTraverser(m_singletonManager, shadowMapIndex);
      renderTraverser.doTraverse(m_renderReflectiveShadowRootNode);
    }

    void GeometryRenderer::rasterizeGeometry()
    {  
      db::ComputeShader *frustumCullingShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_frustumCullingShaderHandle);
      frustumCullingShader->useShader();
      int viewProjectionIndex = -1;
      db::ComputeShader::setUniform(1, GL_INT, &viewProjectionIndex);
      FrustumCullingTraverser frustumCullingTraverser;
      frustumCullingTraverser.doTraverse(m_renderRootNode);
      frustumCullingShader->useNoShader();

      RenderGeometryTraverser renderTraverser(m_singletonManager, -1);
      renderTraverser.doTraverse(m_renderRootNode);
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