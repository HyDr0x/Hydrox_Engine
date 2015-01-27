#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include <DataBase/Mesh.h>

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
    InsertGeometryTraverser::InsertGeometryTraverser(const xBar::IGeometryContainer& geometryContainer, util::SingletonManager *singletonManager) :
      m_geometryContainer(geometryContainer),
      m_inserted(false)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
      m_renderShaderContainer = singletonManager->getService<db::ShaderContainer>();
    }

    InsertGeometryTraverser::~InsertGeometryTraverser()
    {
    }

    bool InsertGeometryTraverser::preTraverse(GroupNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        createNewChildNode(treeNode);
      }

      return true;
    }

    bool InsertGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      m_inserted = treeNode->isMesh(m_meshVertexDeclaration);

      if(m_inserted)
      {
        if(treeNode->getFirstChild() == nullptr)
        {
          createNewChildNode(treeNode);
        }
      }
      else if(treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }

      return m_inserted;
    }

    bool InsertGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      m_inserted = treeNode->isShader(m_shaderHandle);

      if(m_inserted)
      {
        if(treeNode->getFirstChild() == nullptr)
        {
          createNewChildNode(treeNode);
        }
      }
      else if(treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }

      return m_inserted;
    }

    bool InsertGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      m_inserted = treeNode->isTexture(m_textureHandles);

      if(m_inserted)
      {
        if(treeNode->getFirstChild() == nullptr)
        {
          createNewChildNode(treeNode);
        }
      }
      else if(treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }

      return m_inserted;
    }

    void InsertGeometryTraverser::createNewChildNode(GroupNode* parent)
    {
      ShaderNode *treeNode = new ShaderNode();
      treeNode->initialize(m_shaderHandle);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverser::createNewChildNode(TextureNode* parent)
    {
      VertexDeclarationNode *treeNode = new VertexDeclarationNode();
      treeNode->initialize(m_shaderVertexDeclaration, m_meshVertexDeclaration);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverser::createNewChildNode(ShaderNode* parent)
    {
      TextureNode *treeNode = new TextureNode();
      treeNode->initialize(m_textureHandles);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverser::createNewSibling(TextureNode* sibling)
    {
      TextureNode *treeNode = new TextureNode();
      treeNode->initialize(m_textureHandles);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }

    void InsertGeometryTraverser::createNewSibling(ShaderNode* sibling)
    {
      ShaderNode *treeNode = new ShaderNode();
      treeNode->initialize(m_shaderHandle);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }

    void InsertGeometryTraverser::createNewSibling(VertexDeclarationNode* sibling)
    {
      VertexDeclarationNode *treeNode = new VertexDeclarationNode();
      treeNode->initialize(m_shaderVertexDeclaration, m_meshVertexDeclaration);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}