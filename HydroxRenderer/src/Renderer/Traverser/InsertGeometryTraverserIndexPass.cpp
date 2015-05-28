#include "Renderer/Traverser/InsertGeometryTraverserIndexPass.h"

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
    InsertGeometryTraverserIndexPass::InsertGeometryTraverserIndexPass()
    {
    }

    InsertGeometryTraverserIndexPass::~InsertGeometryTraverserIndexPass()
    {
    }

    void InsertGeometryTraverserIndexPass::insertGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<IRenderGroup> sharedRenderGroup, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
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

      switch(mesh->getPrimitiveType())
      {
      case GL_POINTS:
        m_shaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::POINTINDEX, sh::ShaderSlotFlags(m_meshVertexDeclaration.toInt()));
        break;
      case GL_LINES:
        m_shaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::LINEINDEX, sh::ShaderSlotFlags(m_meshVertexDeclaration.toInt()));
        break;
      case GL_TRIANGLES:
      default:
        m_shaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::TRIANGLEINDEX, sh::ShaderSlotFlags(m_meshVertexDeclaration.toInt()));
        break;
      }

      if(m_shaderHandle)
      {
        m_shaderVertexDeclaration = m_renderShaderContainer->getRenderShader(m_shaderHandle).getVertexDeclaration();

        doTraverse(treeNode);
      }
    }

    bool InsertGeometryTraverserIndexPass::preTraverse(RenderNode * treeNode)
    {
      if(!m_stopTraversal && !treeNode->getNextSibling())
      {
        createNewSibling(treeNode);
      }

      return false;
    }

    void InsertGeometryTraverserIndexPass::createNewChildNode(VertexDeclarationNode * parent)
    {
      m_stopTraversal = true;

      util::SharedPointer<RenderNode> treeNode = util::SharedPointer<RenderNode>(new RenderNode(m_sharedRenderGroup));

      parent->setFirstChild(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverserIndexPass::createNewSibling(RenderNode * sibling)
    {
      m_stopTraversal = true;

      util::SharedPointer<RenderNode> treeNode = util::SharedPointer<RenderNode>(new RenderNode(m_sharedRenderGroup));

      sibling->setNextSibling(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(sibling->getParent());
    }
  }
}