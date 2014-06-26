#include "Renderer/Pipeline/GeometryRenderer.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/InsertStaticGeometryTraverser.h"
#include "Renderer/Traverser/InsertSkinnedGeometryTraverser.h"
#include "Renderer/Traverser/RemoveGeometryTraverser.h"
#include "Renderer/Traverser/RenderGeometryTraverser.h"
#include "Renderer/Traverser/FrustumCullingTraverser.h"
#include "Renderer/Traverser/ShadowMapTraverser.h"
#include "Renderer/Traverser/UpdateTraverser.h"
#include "Renderer/Traverser/DeleteTraverser.h"

#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
  namespace renderer
  {
    GeometryRenderer::GeometryRenderer() : m_renderRootNode(nullptr)
    {
    }

    GeometryRenderer::~GeometryRenderer()
    {
      DeleteTraverser deleteTraverser;
      deleteTraverser.doTraverse(m_renderRootNode);

      delete m_renderRootNode;
    }

    void GeometryRenderer::initialize(const RenderOptions& options, util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle, util::ResourceHandle shadowMapGenerationShaderHandle, unsigned int nodeCacheBlockSize)
    {
      m_options = options;

      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glDepthFunc(GL_LESS);
      glClearDepth(1.0f);

      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

      m_renderRootNode = new GroupNode();

      m_singletonManager = singletonManager;

      m_frustumCullingShaderHandle = cullingShaderHandle;
      m_shadowMapGenerationShaderHandle = shadowMapGenerationShaderHandle;

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());
    }

    void GeometryRenderer::addRenderComponent(const xBar::StaticGeometryContainer &staticGeometry)
    {
      InsertStaticGeometryTraverser insertTraverser(staticGeometry, m_options, m_singletonManager);
      insertTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::addRenderComponent(const xBar::SkinnedGeometryContainer &skinnedGeometry)
    {
      InsertSkinnedGeometryTraverser insertTraverser(skinnedGeometry, m_options, m_singletonManager);
      insertTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::removeRenderComponent(const xBar::StaticGeometryContainer &staticGeometry)
    {
      RemoveGeometryTraverser removeTraverser(m_singletonManager, staticGeometry);
      removeTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::removeRenderComponent(const xBar::SkinnedGeometryContainer &skinnedGeometry)
    {
      RemoveGeometryTraverser removeTraverser(m_singletonManager, skinnedGeometry);
      removeTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::generateShadowMap(unsigned int shadowMapIndex)
    {
      FrustumCullingTraverser frustumCullingTraverser(shadowMapIndex);

      db::ComputeShader *frustumCullingShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_frustumCullingShaderHandle);
      frustumCullingShader->useShader();
      frustumCullingTraverser.doTraverse(m_renderRootNode);
      frustumCullingShader->useNoShader();

      db::RenderShader *shadowMapGenerationShader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_shadowMapGenerationShaderHandle);
      ShadowMapTraverser shadowMapTraverser;
      shadowMapGenerationShader->useShader();
      shadowMapTraverser.doTraverse(m_renderRootNode);
      shadowMapGenerationShader->useNoShader();
    }

    void GeometryRenderer::rasterizeGeometry()
    {  
      UpdateTraverser updateTraverser;
      updateTraverser.doTraverse(m_renderRootNode);

      FrustumCullingTraverser frustumCullingTraverser(UINT32_MAX);

      db::ComputeShader *frustumCullingShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_frustumCullingShaderHandle);
      frustumCullingShader->useShader();
      frustumCullingTraverser.doTraverse(m_renderRootNode);
      frustumCullingShader->useNoShader();

      RenderGeometryTraverser renderTraverser(m_singletonManager);
      renderTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(const xBar::StaticGeometryContainer &staticGeometry)>(util::EventManager::OnAddGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void (*)(const xBar::StaticGeometryContainer &staticGeometry), void (GeometryRenderer::*)(const xBar::StaticGeometryContainer &staticGeometry)>(this, &GeometryRenderer::addRenderComponent, util::EventManager::OnAddGeometryNode);

      eventManager->addNewSignal<void (*)(const xBar::SkinnedGeometryContainer &skinnedGeometry)>(util::EventManager::OnAddSkinnedGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void (*)(const xBar::SkinnedGeometryContainer &skinnedGeometry), void (GeometryRenderer::*)(const xBar::SkinnedGeometryContainer &skinnedGeometry)>(this, &GeometryRenderer::addRenderComponent, util::EventManager::OnAddSkinnedGeometryNode);
      
      eventManager->addNewSignal<void (*)(const xBar::StaticGeometryContainer& staticGeometry)>(util::EventManager::OnRemoveGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void (*)(const xBar::StaticGeometryContainer& staticGeometry), void (GeometryRenderer::*)(const xBar::StaticGeometryContainer& staticGeometry)>(this, &GeometryRenderer::removeRenderComponent, util::EventManager::OnRemoveGeometryNode);

      eventManager->addNewSignal<void (*)(const xBar::SkinnedGeometryContainer &skinnedGeometry)>(util::EventManager::OnRemoveSkinnedGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void (*)(const xBar::SkinnedGeometryContainer &skinnedGeometry), void (GeometryRenderer::*)(const xBar::SkinnedGeometryContainer &skinnedGeometry)>(this, &GeometryRenderer::removeRenderComponent, util::EventManager::OnRemoveSkinnedGeometryNode);
    }
  }
}