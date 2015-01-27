#include "Renderer/Pipeline/GeometryRenderer.h"

#include <XBar/IGeometryContainer.h>

#include <DataBase/ShaderContainer.h>

#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/InsertGeometryTraverserRenderPass.h"
#include "Renderer/Traverser/InsertGeometryTraverserIndexPass.h"
#include "Renderer/Traverser/InsertGeometryTraverserShadowPass.h"
#include "Renderer/Traverser/InsertGeometryTraverserReflectiveShadowPass.h"

#include "Renderer/Traverser/RemoveGeometryTraverserRenderPass.h"
#include "Renderer/Traverser/RemoveGeometryTraverserIndexPass.h"
#include "Renderer/Traverser/RemoveGeometryTraverserShadowPass.h"
#include "Renderer/Traverser/RemoveGeometryTraverserReflectiveShadowPass.h"

#include "Renderer/Traverser/UpdateTraverser.h"

#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    GeometryRenderer::GeometryRenderer() :
      m_globalCacheNumber(0)
    {
      m_renderRootNode = new GroupNode();
      m_renderIndexRootNode = new GroupNode();
      m_renderShadowRootNode = new GroupNode();
      m_renderReflectiveShadowRootNode = new GroupNode();
    }

    GeometryRenderer::~GeometryRenderer()
    {
      delete m_renderRootNode;
      delete m_renderIndexRootNode;
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

      m_container = m_singletonManager->getService<db::ShaderContainer>();

      std::vector<util::SharedPointer<SamplerObject>> samplerObjects(db::Material::TEXTURENUMBER);
      samplerObjects[db::Material::DIFFUSETEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR));
      samplerObjects[db::Material::NORMALTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST));
      samplerObjects[db::Material::SPECULARTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST));
      samplerObjects[db::Material::DISPLACEMENTTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST));

      m_renderGeometryTraverser.initialize(m_singletonManager, samplerObjects);
      m_renderIndexGeometryTraverser.initialize(m_singletonManager);
      m_renderShadowGeometryTraverser.initialize(m_singletonManager);

      registerRenderComponentSlots(m_singletonManager->getService<util::EventManager>());
    }

    void GeometryRenderer::addRenderComponent(const xBar::IGeometryContainer& geometry)
    {
      m_globalCacheNumber += m_singletonManager->getService<db::ModelManager>()->getObject(geometry.getMeshHandle())->getCacheCount();

      InsertGeometryTraverserRenderPass insertTraverser(geometry, m_singletonManager);
      insertTraverser.doTraverse(m_renderRootNode);

      util::SharedPointer<IRenderGroup> createdRenderGroup = insertTraverser.getCreatedRenderNode();

      if(createdRenderGroup)
      {
        InsertGeometryTraverserIndexPass insertIndexTraverser(createdRenderGroup, geometry, m_singletonManager);

        insertIndexTraverser.doTraverse(m_renderIndexRootNode);

        InsertGeometryTraverserShadowPass insertShadowTraverser(createdRenderGroup, geometry, m_singletonManager);

        insertShadowTraverser.doTraverse(m_renderShadowRootNode);

        InsertGeometryTraverserReflectiveShadowPass insertReflectiveShadowTraverser(createdRenderGroup, geometry, m_singletonManager);

        insertReflectiveShadowTraverser.doTraverse(m_renderReflectiveShadowRootNode);
      }
    }

    void GeometryRenderer::removeRenderComponent(const xBar::IGeometryContainer& geometry)
    {
      m_globalCacheNumber -= m_singletonManager->getService<db::ModelManager>()->getObject(geometry.getMeshHandle())->getCacheCount();

      RemoveGeometryTraverserRenderPass removeTraverser(m_singletonManager, geometry);
      removeTraverser.doTraverse(m_renderRootNode);

      RemoveGeometryTraverserIndexPass removeIndexTraverser(m_singletonManager, geometry);

      removeIndexTraverser.doTraverse(m_renderIndexRootNode);

      RemoveGeometryTraverserShadowPass removeShadowTraverser(m_singletonManager, geometry);

      removeShadowTraverser.doTraverse(m_renderShadowRootNode);

      RemoveGeometryTraverserReflectiveShadowPass removeReflectiveShadowTraverser(m_singletonManager, geometry);

      removeReflectiveShadowTraverser.doTraverse(m_renderReflectiveShadowRootNode);
    }

    void GeometryRenderer::updateBuffer()
    {
      UpdateTraverser updateTraverser;
      updateTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::frustumCulling(int cameraIndex, RenderPass pass)
    {
      db::ComputeShader *frustumCullingShader = m_singletonManager->getService<db::ComputeShaderManager>()->getObject(m_container->getComputeShader(m_singletonManager, db::ShaderContainer::FRUSTUMCULLING));
      frustumCullingShader->useShader();

      db::ComputeShader::setUniform(1, GL_INT, &cameraIndex);
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

    void GeometryRenderer::generateShadowMap(int cameraIndex, RenderPass pass)
    {
      m_renderShadowGeometryTraverser.setViewProjectionIndex(cameraIndex);

      switch(pass)
      {
      case SHADOWPASS:
        m_renderShadowGeometryTraverser.doTraverse(m_renderShadowRootNode);
        break;
      case REFLECTIVESHADOWPASS:
        m_renderShadowGeometryTraverser.doTraverse(m_renderReflectiveShadowRootNode);
        break;
      }
    }

    void GeometryRenderer::rasterizeIndexGeometry()
    {
      m_renderIndexGeometryTraverser.doTraverse(m_renderIndexRootNode);
    }

    void GeometryRenderer::rasterizeGeometry()
    {  
      m_renderGeometryTraverser.doTraverse(m_renderRootNode);
    }

    unsigned int GeometryRenderer::getGlobalCacheNumber() const
    {
      return m_globalCacheNumber;
    }

    void GeometryRenderer::registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager)
    {
      eventManager->addNewSignal<void(*)(const xBar::IGeometryContainer &geometry)>(util::EventManager::OnAddGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void(*)(const xBar::IGeometryContainer &geometry), void (GeometryRenderer::*)(const xBar::IGeometryContainer &geometry)>(this, &GeometryRenderer::addRenderComponent, util::EventManager::OnAddGeometryNode);
      
      eventManager->addNewSignal<void(*)(const xBar::IGeometryContainer& geometry)>(util::EventManager::OnRemoveGeometryNode);
      eventManager->addSlotToSignal<GeometryRenderer, void(*)(const xBar::IGeometryContainer& geometry), void (GeometryRenderer::*)(const xBar::IGeometryContainer& geometry)>(this, &GeometryRenderer::removeRenderComponent, util::EventManager::OnRemoveGeometryNode);
    }
  }
}