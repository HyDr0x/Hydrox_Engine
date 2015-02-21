#include "Renderer/Traverser/InsertGeometryTraverserIndexPass.h"

#include <DataBase/Mesh.h>
#include <DataBase/ShaderContainer.h>
#include <DataBase/ResourceManager.hpp>

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

    void InsertGeometryTraverserIndexPass::insertGeometry(TreeNode *treeNode, util::SharedPointer<IRenderGroup> sharedRenderGroup, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_sharedRenderGroup = sharedRenderGroup;
      m_geometryContainer = geometryContainer;
      m_inserted = false;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
      m_renderShaderContainer = singletonManager->getService<db::ShaderContainer>();

      db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer->getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      switch(mesh->getPrimitiveType())
      {
      case GL_POINTS:
        m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, db::ShaderContainer::POINTINDEX, m_meshVertexDeclaration);
        break;
      case GL_LINES:
        m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, db::ShaderContainer::LINEINDEX, m_meshVertexDeclaration);
        break;
      case GL_TRIANGLES:
      default:
        m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, db::ShaderContainer::TRIANGLEINDEX, m_meshVertexDeclaration);
        break;
      }

      if(m_shaderHandle)
      {
        m_shaderVertexDeclaration = m_renderShaderManager->getObject(m_shaderHandle)->getVertexDeclaration();

        doTraverse(treeNode);
      }
    }

    bool InsertGeometryTraverserIndexPass::preTraverse(RenderNode* treeNode)
    {
      if(!m_stopTraversal && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }

      return false;
    }

    void InsertGeometryTraverserIndexPass::createNewChildNode(VertexDeclarationNode* parent)
    {
      m_stopTraversal = true;

      RenderNode *treeNode = new RenderNode(m_sharedRenderGroup);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverserIndexPass::createNewSibling(RenderNode* sibling)
    {
      m_stopTraversal = true;

      RenderNode *treeNode = new RenderNode(m_sharedRenderGroup);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}