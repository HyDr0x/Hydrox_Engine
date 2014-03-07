#include "Renderer/Pipeline/GeometryRenderer.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"
#include "Renderer/Traverser/RemoveGeometryTraverser.h"
#include "Renderer/Traverser/RenderGeometryTraverser.h"
#include "Renderer/Traverser/DeleteTraverser.h"

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

      glDeleteVertexArrays(1, &m_meshVAO);
    }

    void GeometryRenderer::initialize(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle, unsigned int nodeCacheBlockSize)
    {
      m_maxMaterials = maxMaterials;
      m_maxGeometry = maxGeometry;
      m_maxBones = maxBones;

      glEnable(GL_DEPTH_TEST);
	    glDepthMask(GL_TRUE);
	    glDepthFunc(GL_GREATER);

      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);

	    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	    glClearDepth(0.0f);

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

    void GeometryRenderer::addRenderComponent(sg::GeoNode *node)
    {
      InsertGeometryTraverser insertTraverser(m_maxMaterials, m_maxGeometry, m_maxBones, m_singletonManager, m_renderNodes, m_frustumCullingShaderHandle);
      insertTraverser.setNode(node);
      insertTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::removeRenderComponent(sg::GeoNode *node)
    {
      RemoveGeometryTraverser removeTraverser(m_singletonManager, m_renderNodes);
      removeTraverser.setNode(node);
      removeTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::rasterizeGeometry()
    {  
      //frustumCulling();

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      glBindVertexArray(m_meshVAO);

      RenderGeometryTraverser renderTraverser(m_singletonManager);
      renderTraverser.doTraverse(m_renderRootNode);

      glBindVertexArray(0);
    }

    void GeometryRenderer::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(sg::GeoNode *node)>(util::EventManager::OnAddGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void (*)(sg::GeoNode *node), void (GeometryRenderer::*)(sg::GeoNode *node)>(this, &GeometryRenderer::addRenderComponent, util::EventManager::OnAddGeometryNode);

      eventManager->addNewSignal<void (*)(sg::GeoNode *node)>(util::EventManager::OnRemoveGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void (*)(sg::GeoNode *node), void (GeometryRenderer::*)(sg::GeoNode *node)>(this, &GeometryRenderer::removeRenderComponent, util::EventManager::OnRemoveGeometryNode);
    }

    void GeometryRenderer::frustumCulling()
    {
      ComputeShader *frustumCullingShader = m_singletonManager->getService<ComputeShaderManager>()->getObject(m_frustumCullingShaderHandle);
      frustumCullingShader->useShader();

      for(std::list<RenderNode*>::const_iterator nodeIterator = m_renderNodes.begin(); nodeIterator != m_renderNodes.end(); nodeIterator++)
      {
        (*nodeIterator)->frustumCulling();
      }

      frustumCullingShader->useNoShader();
    }
  }
}