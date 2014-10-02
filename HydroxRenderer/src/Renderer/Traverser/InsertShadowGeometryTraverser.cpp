#include "Renderer/Traverser/InsertShadowGeometryTraverser.h"

#include <DataBase/Mesh.h>

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
  namespace renderer
  {
    InsertShadowGeometryTraverser::InsertShadowGeometryTraverser(const xBar::IGeometryContainer& geometryContainer, const RenderOptions& options, util::SingletonManager *singletonManager) :
      m_geometryContainer(geometryContainer),
      m_options(options),
      m_singletonManager(singletonManager),
      m_inserted(false)
    {
    }

    InsertShadowGeometryTraverser::~InsertShadowGeometryTraverser()
    {
    }

    void InsertShadowGeometryTraverser::initialize(util::ResourceHandle staticShadowMapGenerationShaderHandle, util::ResourceHandle animatedShadowMapGenerationShaderHandle)
    {
      db::Mesh *mesh = m_singletonManager->getService<db::ModelManager>()->getObject(m_geometryContainer.getMeshHandle());
      m_vertexDeclaration = mesh->getVertexDeclarationFlags();
      m_primitiveType = mesh->getPrimitiveType();
      m_vertexStride = mesh->getVertexStride();

      if(mesh->getVertexDeclarationFlags() & db::Mesh::MODEL_BONE_WEIGHTS)
      {
        m_shaderHandle = animatedShadowMapGenerationShaderHandle;
      }
      else
      {
        m_shaderHandle = staticShadowMapGenerationShaderHandle;
      }
    }

    bool InsertShadowGeometryTraverser::preTraverse(GroupNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        createNewChildNode(treeNode);
      }

      return true;
    }

    void InsertShadowGeometryTraverser::postTraverse(GroupNode* treeNode)
    {
    }

    bool InsertShadowGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      m_inserted = treeNode->isMesh(m_vertexDeclaration);

      if(treeNode->getFirstChild() == nullptr && m_inserted)
      {
        createNewChildNode(treeNode);
      }

      return m_inserted;
    }

    void InsertShadowGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    bool InsertShadowGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      m_inserted = treeNode->isShader(m_shaderHandle);

      if(treeNode->getFirstChild() == nullptr && m_inserted)
      {
        createNewChildNode(treeNode);
      }

      return m_inserted;
    }

    void InsertShadowGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    bool InsertShadowGeometryTraverser::preTraverse(IRenderNode* treeNode)
    {
      m_inserted = false;
      if(treeNode->insertGeometry(m_geometryContainer))
      {
        m_stopTraversal = true;
        m_inserted = true;
      }

      return false;
    }
    void InsertShadowGeometryTraverser::postTraverse(IRenderNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    void InsertShadowGeometryTraverser::createNewChildNode(TreeNode* parent)
    {
    }

    void InsertShadowGeometryTraverser::createNewChildNode(GroupNode* parent)
    {
      ShaderNode *treeNode = new ShaderNode();
      treeNode->initialize(m_shaderHandle);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertShadowGeometryTraverser::createNewChildNode(ShaderNode* parent)
    {
      VertexDeclarationNode *treeNode = new VertexDeclarationNode();
      treeNode->initialize(m_vertexDeclaration);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertShadowGeometryTraverser::createNewChildNode(VertexDeclarationNode* parent)
    {
      IRenderNode *treeNode = RenderNodeFactory::createShadowRenderNode(m_nodeType, m_options, m_primitiveType, m_vertexStride, m_singletonManager);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertShadowGeometryTraverser::createNewSibling(TreeNode* sibling)
    {
    }

    void InsertShadowGeometryTraverser::createNewSibling(ShaderNode* sibling)
    {
      ShaderNode *treeNode = new ShaderNode();
      treeNode->initialize(m_shaderHandle);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }

    void InsertShadowGeometryTraverser::createNewSibling(VertexDeclarationNode* sibling)
    {
      VertexDeclarationNode *treeNode = new VertexDeclarationNode();
      treeNode->initialize(m_vertexDeclaration);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }

    void InsertShadowGeometryTraverser::createNewSibling(IRenderNode* sibling)
    {
      IRenderNode *treeNode = RenderNodeFactory::createShadowRenderNode(m_nodeType, m_options, m_primitiveType, m_vertexStride, m_singletonManager);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}