#include "Renderer/Pipeline/GeometryRenderer.h"

#include <Shader/ShaderContainer.h>

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/VertexDeclarationNode.h"

#include "Renderer/Traverser/InsertGeometryTraverserDebug.h"
#include "Renderer/Traverser/InsertGeometryTraverserRenderPass.h"
#include "Renderer/Traverser/InsertGeometryTraverserIndexPass.h"
#include "Renderer/Traverser/InsertGeometryTraverserShadowPass.h"
#include "Renderer/Traverser/InsertGeometryTraverserReflectiveShadowPass.h"
#include "Renderer/Traverser/InsertGeometryTraverserIndirectLightingPass.h"

#include "Renderer/Traverser/RemoveGeometryTraverserDebug.h"
#include "Renderer/Traverser/RemoveGeometryTraverserRenderPass.h"
#include "Renderer/Traverser/RemoveGeometryTraverserIndexPass.h"
#include "Renderer/Traverser/RemoveGeometryTraverserShadowPass.h"
#include "Renderer/Traverser/RemoveGeometryTraverserReflectiveShadowPass.h"
#include "Renderer/Traverser/RemoveGeometryTraverserIndirectLightingPass.h"

#include "Renderer/Traverser/UpdateTraverser.h"

#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

#include "Renderer/Pipeline/RenderOptions.h"

#include <Utilities/Timer/Timer.h>

namespace he
{
  namespace renderer
  {
    GeometryRenderer::GeometryRenderer() :
      m_globalCacheNumber(0)
    {
      m_renderRootNode = util::SharedPointer<GroupNode>(new GroupNode).dynamic_pointer_cast<TreeNode>();
      m_renderDebugRootNode = util::SharedPointer<GroupNode>(new GroupNode).dynamic_pointer_cast<TreeNode>();
      m_renderIndexRootNode = util::SharedPointer<GroupNode>(new GroupNode).dynamic_pointer_cast<TreeNode>();
      m_renderShadowRootNode = util::SharedPointer<GroupNode>(new GroupNode).dynamic_pointer_cast<TreeNode>();
      m_renderReflectiveShadowRootNode = util::SharedPointer<GroupNode>(new GroupNode).dynamic_pointer_cast<TreeNode>();
      m_renderIndirectLightRootNode = util::SharedPointer<GroupNode>(new GroupNode).dynamic_pointer_cast<TreeNode>();
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
      samplerObjects[db::Material::NORMALTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST));
      samplerObjects[db::Material::SPECULARTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST));
      samplerObjects[db::Material::DISPLACEMENTTEX] = util::SharedPointer<SamplerObject>(new SamplerObject(GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST));

      m_renderGeometryTraverser.initialize(m_singletonManager, samplerObjects);
      m_renderGeometryTraverserDebug.initialize(m_singletonManager, samplerObjects);
      m_renderIndexGeometryTraverser.initialize(m_singletonManager);
      m_renderShadowGeometryTraverser.initialize(m_singletonManager);
      m_renderReflectiveShadowGeometryTraverser.initialize(m_singletonManager);
      m_renderIndirectLightingGeometryTraverser.initialize(m_singletonManager, normalMap, materialMap);

      registerRenderComponentSlots(m_singletonManager->getService<util::EventManager>());
    }

    void GeometryRenderer::addRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry)
    {
      if(!m_singletonManager->getService<db::MaterialManager>()->getObject(geometry->getMaterialHandle())->getDebug())
      {
        InsertGeometryTraverserRenderPass insertTraverser;
        insertTraverser.insertGeometry(m_renderRootNode, geometry, m_singletonManager);

        util::SharedPointer<IRenderGroup> createdRenderGroup = insertTraverser.getCreatedRenderNode();

        if(createdRenderGroup)
        {
          InsertGeometryTraverserIndexPass insertIndexTraverser;
          insertIndexTraverser.insertGeometry(m_renderIndexRootNode, createdRenderGroup, geometry, m_singletonManager);

          InsertGeometryTraverserShadowPass insertShadowTraverser;
          insertShadowTraverser.insertGeometry(m_renderShadowRootNode, createdRenderGroup, geometry, m_singletonManager);

          InsertGeometryTraverserReflectiveShadowPass insertReflectiveShadowTraverser;
          insertReflectiveShadowTraverser.insertGeometry(m_renderReflectiveShadowRootNode, createdRenderGroup, geometry, m_singletonManager);

          InsertGeometryTraverserIndirectLightingPass insertGeometryTraverserIndirectLightingPass;
          insertGeometryTraverserIndirectLightingPass.insertGeometry(m_renderIndirectLightRootNode, createdRenderGroup, geometry, m_singletonManager);
        }
      }
      else
      {
        InsertGeometryTraverserDebug insertDebugTraverser;
        insertDebugTraverser.insertGeometry(m_renderDebugRootNode, geometry, m_singletonManager);
      }
    }

    void GeometryRenderer::removeRenderComponent(util::SharedPointer<const xBar::IGeometryContainer> geometry)
    {
      if(!m_singletonManager->getService<db::MaterialManager>()->getObject(geometry->getMaterialHandle())->getDebug())
      {
        RemoveGeometryTraverserRenderPass removeTraverser;
        removeTraverser.removeGeometry(m_renderRootNode, geometry, m_singletonManager);

        RemoveGeometryTraverserIndexPass removeIndexTraverser;
        removeIndexTraverser.removeGeometry(m_renderIndexRootNode, geometry, m_singletonManager);

        RemoveGeometryTraverserShadowPass removeShadowTraverser;
        removeShadowTraverser.removeGeometry(m_renderShadowRootNode, geometry, m_singletonManager);

        RemoveGeometryTraverserReflectiveShadowPass removeReflectiveShadowTraverser;
        removeReflectiveShadowTraverser.removeGeometry(m_renderReflectiveShadowRootNode, geometry, m_singletonManager);

        RemoveGeometryTraverserIndirectLightingPass removeGeometryTraverserIndirectLightingPass;
        removeGeometryTraverserIndirectLightingPass.removeGeometry(m_renderIndirectLightRootNode, geometry, m_singletonManager);
      }
      else
      {
        RemoveGeometryTraverserDebug removeDebugTraverser;
        removeDebugTraverser.removeGeometry(m_renderDebugRootNode, geometry, m_singletonManager);
      }
    }

    void GeometryRenderer::updateBuffer()
    {
      UpdateTraverser updateTraverser;
      updateTraverser.doTraverse(m_renderRootNode);

      m_globalCacheNumber = updateTraverser.getGlobalCacheNumber();

      m_proxyLightTextureResolution = static_cast<unsigned int>(pow(2.0f, ceil(log(sqrt(float(m_globalCacheNumber))) / log(2.0f))));
      m_renderIndirectLightingGeometryTraverser.setGlobalBufferResolution(m_proxyLightTextureResolution);
      m_renderIndexGeometryTraverser.setGlobalBufferResolution(m_proxyLightTextureResolution);
    }

    void GeometryRenderer::frustumCulling(int cameraIndex, RenderPass pass)
    {
      const sh::ComputeShader& frustumCullingShader = m_container->getComputeShader(sh::ShaderContainer::FRUSTUMCULLING);
      frustumCullingShader.useShader();

      sh::ComputeShader::setUniform(1, GL_INT, &cameraIndex);
      FrustumCullingTraverser frustumCullingTraverser;
      
      switch(pass)
      {
        case SHADOWPASS:
          frustumCullingTraverser.doTraverse(m_renderShadowRootNode.dynamic_pointer_cast<const TreeNode>());
          break;
        case REFLECTIVESHADOWPASS:
          frustumCullingTraverser.doTraverse(m_renderReflectiveShadowRootNode.dynamic_pointer_cast<const TreeNode>());
          break;
        case VIEWPASS:
          frustumCullingTraverser.doTraverse(m_renderRootNode.dynamic_pointer_cast<const TreeNode>());
          break;
      }

      frustumCullingShader.useNoShader();
    }

    void GeometryRenderer::generateShadowMap(int cameraIndex)
    {
      m_renderShadowGeometryTraverser.setViewProjectionIndex(cameraIndex);
      m_renderShadowGeometryTraverser.doTraverse(m_renderShadowRootNode);
    }

    void GeometryRenderer::generateReflectiveShadowMap(int cameraIndex)
    {
      //CPUTIMER("cpuCompute", 0)
      //GPUTIMER("gpuCompute", 1)
      m_renderReflectiveShadowGeometryTraverser.setViewProjectionIndex(cameraIndex);
      m_renderReflectiveShadowGeometryTraverser.doTraverse(m_renderReflectiveShadowRootNode);
    }

    void GeometryRenderer::generateIndirectLightMap()
    {
      m_renderIndirectLightingGeometryTraverser.doTraverse(m_renderIndirectLightRootNode);
    }

    void GeometryRenderer::rasterizeIndexGeometry()
    {
      m_renderIndexGeometryTraverser.doTraverse(m_renderIndexRootNode);
    }

    void GeometryRenderer::rasterizeGeometry()
    {
      m_renderGeometryTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRenderer::rasterizeDebugGeometry()
    {
      UpdateTraverser updateTraverser;
      updateTraverser.doTraverse(m_renderDebugRootNode);

      m_renderGeometryTraverserDebug.doTraverse(m_renderDebugRootNode);
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