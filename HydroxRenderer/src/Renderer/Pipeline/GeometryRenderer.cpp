#include "Renderer/Pipeline/GeometryRenderer.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Pipeline/RenderingOptions.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/InsertStaticGeometryTraverser.h"
#include "Renderer/Traverser/InsertSkinnedGeometryTraverser.h"
#include "Renderer/Traverser/RemoveGeometryTraverser.h"
#include "Renderer/Traverser/RenderGeometryTraverser.h"
#include "Renderer/Traverser/FrustumCullingTraverser.h"
#include "Renderer/Traverser/DeleteTraverser.h"

#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
	namespace renderer
	{
    GeometryRenderer::GeometryRenderer(RenderOptions& options) : m_renderRootNode(nullptr), m_options(options)
    {
    }

    GeometryRenderer::~GeometryRenderer()
    {
      DeleteTraverser deleteTraverser;
      deleteTraverser.doTraverse(m_renderRootNode);

      delete m_renderRootNode;

      glDeleteVertexArrays(1, &m_meshVAO);
    }

    void GeometryRenderer::initialize(util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle, unsigned int nodeCacheBlockSize)
    {
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

      glGenVertexArrays(1, &m_meshVAO);
      glBindVertexArray(m_meshVAO);

      glVertexAttribFormat(RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribFormat(RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 3>));
      glVertexAttribFormat(RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BINORMAL, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 4>) + 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::COLOR, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(util::Vector<float, 4>) + 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));

      glVertexAttribBinding(RenderShader::POSITION, 0);
      glVertexAttribBinding(RenderShader::TEXTURE0, 0);
      glVertexAttribBinding(RenderShader::NORMAL, 0);
      glVertexAttribBinding(RenderShader::BINORMAL, 0);
      glVertexAttribBinding(RenderShader::BONEWEIGHTS, 0);
      glVertexAttribBinding(RenderShader::BONEINDICES, 0);
      glVertexAttribBinding(RenderShader::COLOR, 0);

      glBindVertexArray(0);

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

    void GeometryRenderer::rasterizeGeometry() const
    {  
      FrustumCullingTraverser frustumCullingTraverser;

      ComputeShader *frustumCullingShader = m_singletonManager->getService<ComputeShaderManager>()->getObject(m_frustumCullingShaderHandle);
      frustumCullingShader->useShader();
      frustumCullingTraverser.doTraverse(m_renderRootNode);
      frustumCullingShader->useNoShader();

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      glBindVertexArray(m_meshVAO);

      RenderGeometryTraverser renderTraverser(m_singletonManager);
      renderTraverser.doTraverse(m_renderRootNode);

      glBindVertexArray(0);
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