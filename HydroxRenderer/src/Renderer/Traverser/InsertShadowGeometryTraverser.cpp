#include "Renderer/Traverser/InsertShadowGeometryTraverser.h"

#include <DataBase/Mesh.h>

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

#include "Renderer/Pipeline/RenderShaderContainer.h"
namespace he
{
  namespace renderer
  {
    InsertShadowGeometryTraverser::InsertShadowGeometryTraverser(util::SharedPointer<IRenderGroup> renderGroup,
      const xBar::IGeometryContainer& geometryContainer,
      util::SingletonManager *singletonManager,
      util::ResourceHandle staticShadowMapGenerationShaderHandle,
      util::ResourceHandle skinnedShadowMapGenerationShaderHandle) :
      m_renderGroup(renderGroup),
      m_singletonManager(singletonManager),
      m_inserted(false)
    {
      db::Mesh *mesh = m_singletonManager->getService<db::ModelManager>()->getObject(geometryContainer.getMeshHandle());
      m_vertexDeclaration = mesh->getVertexDeclarationFlags();

      if(geometryContainer.getNodeType() == util::Flags<xBar::RenderNodeType>(xBar::SKINNEDNODE))
      {
        m_shaderHandle = skinnedShadowMapGenerationShaderHandle;
      }
      else
      {
        m_shaderHandle = staticShadowMapGenerationShaderHandle;
      }
    }

    InsertShadowGeometryTraverser::~InsertShadowGeometryTraverser()
    {
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

    bool InsertShadowGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      m_inserted = treeNode->isMesh(m_vertexDeclaration);

      if(treeNode->getFirstChild() == nullptr && m_inserted)
      {
        createNewChildNode(treeNode);
        m_stopTraversal = true;
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

    bool InsertShadowGeometryTraverser::preTraverse(RenderNode* treeNode)
    {
      m_inserted = false;

      return m_inserted;
    }

    void InsertShadowGeometryTraverser::postTraverse(RenderNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
        m_stopTraversal = true;
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
      RenderNode *treeNode = new RenderNode(m_renderGroup);

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

    void InsertShadowGeometryTraverser::createNewSibling(RenderNode* sibling)
    {
      RenderNode *treeNode = new RenderNode(m_renderGroup);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}