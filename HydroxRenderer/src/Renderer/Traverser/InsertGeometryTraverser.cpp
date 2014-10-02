#include "Renderer/Traverser/InsertGeometryTraverser.h"

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
    InsertGeometryTraverser::InsertGeometryTraverser(const xBar::IGeometryContainer& geometryContainer, const RenderOptions& options, util::SingletonManager *singletonManager) :
      m_geometryContainer(geometryContainer),
      m_options(options),
      m_singletonManager(singletonManager),
      m_inserted(false)
    {
      db::Mesh *mesh = m_singletonManager->getService<db::ModelManager>()->getObject(m_geometryContainer.getMeshHandle());
      m_vertexDeclaration = mesh->getVertexDeclarationFlags();
      m_primitiveType = mesh->getPrimitiveType();
      m_vertexStride = mesh->getVertexStride();

      db::Material *material = m_singletonManager->getService<db::MaterialManager>()->getObject(m_geometryContainer.getMaterialHandle());
      m_shaderHandle = material->getShaderHandle();

      m_textureHandles.resize(db::Material::TEXTURETYPENUM);

      for(unsigned int i = 0; i < m_textureHandles.size(); i++)
      {
        unsigned int texNum = material->getTextureNumber((db::Material::TextureType)i);
        m_textureHandles[i].resize(texNum);

        for(unsigned int j = 0; j < texNum; j++)
        {
          m_textureHandles[i][j] = material->getTextureHandle((db::Material::TextureType)i, j);
        }
      }
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

    void InsertGeometryTraverser::postTraverse(GroupNode* treeNode)
    {
    }

    bool InsertGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        createNewChildNode(treeNode);
      }

      m_inserted = treeNode->isMesh(m_vertexDeclaration);

      return m_inserted;
    }

    void InsertGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    bool InsertGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        createNewChildNode(treeNode);
      }

      m_inserted = treeNode->isShader(m_shaderHandle);

      return m_inserted;
    }

    void InsertGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    bool InsertGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        createNewChildNode(treeNode);
      }

      m_inserted = treeNode->isTexture(m_textureHandles);

      return m_inserted;
    }

    void InsertGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    bool InsertGeometryTraverser::preTraverse(IRenderNode* treeNode)
    {
      m_inserted = false;
      if(treeNode->insertGeometry(m_geometryContainer))
      {
        m_stopTraversal = true;
        m_inserted = true;
      }

      return false;
    }

    void InsertGeometryTraverser::postTraverse(IRenderNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    void InsertGeometryTraverser::createNewChildNode(TreeNode* parent)
    {
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
      treeNode->initialize(m_vertexDeclaration);

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

    void InsertGeometryTraverser::createNewChildNode(VertexDeclarationNode* parent)
    {
      IRenderNode *treeNode = RenderNodeFactory::createRenderNode(m_nodeType, m_options, m_primitiveType, m_vertexStride, m_singletonManager);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverser::createNewSibling(TreeNode* sibling)
    {
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
      treeNode->initialize(m_vertexDeclaration);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }

    void InsertGeometryTraverser::createNewSibling(IRenderNode* sibling)
    {
      IRenderNode *treeNode = RenderNodeFactory::createRenderNode(m_nodeType, m_options, m_primitiveType, m_vertexStride, m_singletonManager);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}