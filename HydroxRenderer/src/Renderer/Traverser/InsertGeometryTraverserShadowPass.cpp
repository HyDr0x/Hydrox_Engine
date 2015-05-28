#include "Renderer/Traverser/InsertGeometryTraverserShadowPass.h"

#include <DataBase/Mesh.h>
#include <DataBase/ResourceManager.hpp>

#include <Shader/ShaderContainer.h>

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    InsertGeometryTraverserShadowPass::InsertGeometryTraverserShadowPass()
    {
    }

    InsertGeometryTraverserShadowPass::~InsertGeometryTraverserShadowPass()
    {
    }

    void InsertGeometryTraverserShadowPass::insertGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<IRenderGroup> sharedRenderGroup, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_sharedRenderGroup = sharedRenderGroup;
      m_geometryContainer = geometryContainer;
      m_inserted = false;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderContainer = singletonManager->getService<sh::ShaderContainer>();

      db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer->getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      m_shaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::SHADOW, sh::ShaderSlotFlags(m_meshVertexDeclaration.toInt()));

      if(m_shaderHandle)
      {
        m_shaderVertexDeclaration = m_renderShaderContainer->getRenderShader(m_shaderHandle).getVertexDeclaration();

        doTraverse(treeNode);
      }
    }

    bool InsertGeometryTraverserShadowPass::preTraverse(RenderNode * treeNode)
    {
      m_inserted = false;

      if(!treeNode->getNextSibling())
      {
        createNewSibling(treeNode);
        m_stopTraversal = m_inserted = true;
      }

      return false;
    }

    void InsertGeometryTraverserShadowPass::createNewChildNode(ShaderNode * parent)
    {
      util::SharedPointer<VertexDeclarationNode> treeNode = util::SharedPointer<VertexDeclarationNode>(new VertexDeclarationNode());
      treeNode->initialize(m_shaderVertexDeclaration, m_meshVertexDeclaration);

      parent->setFirstChild(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverserShadowPass::createNewChildNode(VertexDeclarationNode * parent)
    {
      util::SharedPointer<RenderNode> treeNode = util::SharedPointer<RenderNode>(new RenderNode(m_sharedRenderGroup));

      parent->setFirstChild(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverserShadowPass::createNewSibling(RenderNode * sibling)
    {
      util::SharedPointer<RenderNode> treeNode = util::SharedPointer<RenderNode>(new RenderNode(m_sharedRenderGroup));

      sibling->setNextSibling(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(sibling->getParent());
    }
  }
}