#include "Renderer/Traverser/InsertReflectiveShadowGeometryTraverser.h"

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
    InsertReflectiveShadowGeometryTraverser::InsertReflectiveShadowGeometryTraverser(util::SharedPointer<IRenderGroup> renderGroup,
      const xBar::IGeometryContainer& geometryContainer,
      util::SingletonManager *singletonManager,
      RenderShaderContainer *container) :
      m_renderGroup(renderGroup),
      m_singletonManager(singletonManager),
      m_inserted(false)
    {
      db::Mesh *mesh = m_singletonManager->getService<db::ModelManager>()->getObject(geometryContainer.getMeshHandle());
      m_vertexDeclaration = mesh->getVertexDeclarationFlags();

      db::Material *material = m_singletonManager->getService<db::MaterialManager>()->getObject(geometryContainer.getMaterialHandle());

      m_shaderHandle = material->getShadowShaderHandle();

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

    InsertReflectiveShadowGeometryTraverser::~InsertReflectiveShadowGeometryTraverser()
    {
    }

    bool InsertReflectiveShadowGeometryTraverser::preTraverse(GroupNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        createNewChildNode(treeNode);
      }

      return true;
    }

    void InsertReflectiveShadowGeometryTraverser::postTraverse(GroupNode* treeNode)
    {
    }

    bool InsertReflectiveShadowGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      m_inserted = treeNode->isShader(m_shaderHandle);

      if(treeNode->getFirstChild() == nullptr && m_inserted)
      {
        createNewChildNode(treeNode);
      }

      return m_inserted;
    }

    void InsertReflectiveShadowGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    bool InsertReflectiveShadowGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      m_inserted = treeNode->isMesh(m_vertexDeclaration);

      if(treeNode->getFirstChild() == nullptr && m_inserted)
      {
        createNewChildNode(treeNode);
        m_stopTraversal = true;
      }

      return m_inserted;
    }

    void InsertReflectiveShadowGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    bool InsertReflectiveShadowGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        createNewChildNode(treeNode);
      }

      m_inserted = treeNode->isTexture(m_textureHandles);

      return m_inserted;
    }

    void InsertReflectiveShadowGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }
    }

    bool InsertReflectiveShadowGeometryTraverser::preTraverse(RenderNode* treeNode)
    {
      m_inserted = false;

      return m_inserted;
    }

    void InsertReflectiveShadowGeometryTraverser::postTraverse(RenderNode* treeNode)
    {
      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
        m_stopTraversal = true;
      }
    }

    void InsertReflectiveShadowGeometryTraverser::createNewChildNode(TreeNode* parent)
    {
    }

    void InsertReflectiveShadowGeometryTraverser::createNewChildNode(GroupNode* parent)
    {
      ShaderNode *treeNode = new ShaderNode();
      treeNode->initialize(m_shaderHandle);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertReflectiveShadowGeometryTraverser::createNewChildNode(ShaderNode* parent)
    {
      TextureNode *treeNode = new TextureNode();
      treeNode->initialize(m_textureHandles);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertReflectiveShadowGeometryTraverser::createNewChildNode(TextureNode* parent)
    {
      VertexDeclarationNode *treeNode = new VertexDeclarationNode();
      treeNode->initialize(m_vertexDeclaration);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertReflectiveShadowGeometryTraverser::createNewChildNode(VertexDeclarationNode* parent)
    {
      RenderNode *treeNode = new RenderNode(m_renderGroup);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertReflectiveShadowGeometryTraverser::createNewSibling(TreeNode* sibling)
    {
    }

    void InsertReflectiveShadowGeometryTraverser::createNewSibling(ShaderNode* sibling)
    {
      ShaderNode *treeNode = new ShaderNode();
      treeNode->initialize(m_shaderHandle);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }

    void InsertReflectiveShadowGeometryTraverser::createNewSibling(TextureNode* sibling)
    {
      TextureNode *treeNode = new TextureNode();
      treeNode->initialize(m_textureHandles);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }

    void InsertReflectiveShadowGeometryTraverser::createNewSibling(VertexDeclarationNode* sibling)
    {
      VertexDeclarationNode *treeNode = new VertexDeclarationNode();
      treeNode->initialize(m_vertexDeclaration);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }

    void InsertReflectiveShadowGeometryTraverser::createNewSibling(RenderNode* sibling)
    {
      RenderNode *treeNode = new RenderNode(m_renderGroup);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}