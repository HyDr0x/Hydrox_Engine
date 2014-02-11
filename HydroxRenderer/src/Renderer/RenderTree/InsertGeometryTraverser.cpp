#include "Renderer/RenderTree/InsertGeometryTraverser.h"

#include "Renderer/RenderTree/TreeNode.h"
#include "Renderer/RenderTree/GroupNode.h"
#include "Renderer/RenderTree/VertexDeclarationNode.h"
#include "Renderer/RenderTree/ShaderNode.h"
#include "Renderer/RenderTree/TextureNode.h"
#include "Renderer/RenderTree/StaticRenderNode.h"
#include "Renderer/RenderTree/AnimatedRenderNode.h"

namespace he
{
	namespace renderer
	{
    InsertGeometryTraverser::InsertGeometryTraverser(ModelManager *modelManager, MaterialManager *materialManager, RenderShaderManager *renderShaderManager) : 
      m_modelManager(modelManager),
      m_materialManager(materialManager),
      m_renderShaderManager(renderShaderManager)
    {
      m_node = nullptr;
      m_parentNode = nullptr;
    }

    InsertGeometryTraverser::~InsertGeometryTraverser()
    {
    }

    void InsertGeometryTraverser::setNode(sg::GeoNode *node)
    {
      m_node = node;

      m_vertexDeclaration = m_modelManager->getObject(m_node->getMeshHandle())->getVertexDeclarationFlags();

      Material *material = m_materialManager->getObject(m_node->getMaterialHandle());
      m_shaderHandle = material->getShaderHandle();

      m_textureHandles.resize(4);

      unsigned int texNum = material->getTextureNumber(Material::DIFFUSETEX);
      m_textureHandles[Material::DIFFUSETEX].resize(texNum);

      for(unsigned int i = 0; i < texNum; i++)
      {
        m_textureHandles[Material::DIFFUSETEX][i] = material->getTextureHandle(Material::DIFFUSETEX, i);
      }

      texNum = material->getTextureNumber(Material::NORMALTEX);
      m_textureHandles[Material::NORMALTEX].resize(texNum);

      for(unsigned int i = 0; i < texNum; i++)
      {
        m_textureHandles[Material::NORMALTEX][i] = material->getTextureHandle(Material::NORMALTEX, i);
      }

      texNum = material->getTextureNumber(Material::SPECULARTEX);
      m_textureHandles[Material::SPECULARTEX].resize(texNum);

      for(unsigned int i = 0; i < texNum; i++)
      {
        m_textureHandles[Material::SPECULARTEX][i] = material->getTextureHandle(Material::SPECULARTEX, i);
      }

      texNum = material->getTextureNumber(Material::DISPLACEMENTTEX);
      m_textureHandles[Material::DISPLACEMENTTEX].resize(texNum);

      for(unsigned int i = 0; i < texNum; i++)
      {
        m_textureHandles[Material::DISPLACEMENTTEX][i] = material->getTextureHandle(Material::DISPLACEMENTTEX, i);
      }
    }

    void InsertGeometryTraverser::doTraverse(TreeNode* treeNode)
    {
      if(treeNode != nullptr)
      {
        if(treeNode->preTraverse(this))
        {
          m_parentNode = treeNode;
          doTraverseDown(treeNode->getFirstChild());
        }
        treeNode->postTraverse(this);
      }
    }

    void InsertGeometryTraverser::doTraverseDown(TreeNode* treeNode)
    {
      while(true)
      {
        if(treeNode == nullptr)
        {
          treeNode = m_parentNode->createNewNode(this);

          if(treeNode == nullptr)
          {
            return;
          }
        }

        if(treeNode->preTraverse(this))
        {
          m_parentNode = treeNode;
          doTraverseDown(treeNode->getFirstChild());

          return;
        }

        treeNode->postTraverse(this);
        treeNode = treeNode->getNextSibling();
      }
    }

    bool InsertGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return treeNode->isMesh(m_vertexDeclaration);
    }

    void InsertGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
    }

    bool InsertGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      return treeNode->isShader(m_shaderHandle);
    }

    void InsertGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
    }

    bool InsertGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      return treeNode->isTexture(m_textureHandles);
    }

    void InsertGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
    }

    bool InsertGeometryTraverser::preTraverse(StaticRenderNode* treeNode)
    {
      treeNode->insertGeometry(m_node);

      return true;
    }

    void InsertGeometryTraverser::postTraverse(StaticRenderNode* treeNode)
    {
    }

    bool InsertGeometryTraverser::preTraverse(AnimatedRenderNode* treeNode)
    {
      treeNode->insertGeometry(m_node);

      return true;
    }

    void InsertGeometryTraverser::postTraverse(AnimatedRenderNode* treeNode)
    {
    }

    TreeNode* InsertGeometryTraverser::createNewNode(TreeNode* parent)
    {
      return nullptr;
    }

    TreeNode* InsertGeometryTraverser::createNewNode(GroupNode* parent)
    {
      VertexDeclarationNode *treeNode = new VertexDeclarationNode();
      treeNode->initialize(m_vertexDeclaration);

      TreeNode *sibling = parent->getFirstChild();
      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
      treeNode->setNextSibling(sibling);

      return treeNode;
    }

    TreeNode* InsertGeometryTraverser::createNewNode(VertexDeclarationNode* parent)
    {
      ShaderNode *treeNode = new ShaderNode();
      treeNode->initialize(m_shaderHandle);

      TreeNode *sibling = parent->getFirstChild();
      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
      treeNode->setNextSibling(sibling);

      return treeNode;
    }

    TreeNode* InsertGeometryTraverser::createNewNode(ShaderNode* parent)
    {
      TextureNode *treeNode = new TextureNode();
      treeNode->initialize(m_textureHandles);

      TreeNode *sibling = parent->getFirstChild();
      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
      treeNode->setNextSibling(sibling);

      return treeNode;
    }

    TreeNode* InsertGeometryTraverser::createNewNode(TextureNode* parent)
    {
      RenderNode *treeNode;
      if(dynamic_cast<sg::AnimatedGeoNode*>(m_node) != nullptr)
      {
        treeNode = new AnimatedRenderNode();
      }
      else
      {
        treeNode = new StaticRenderNode();
      }

      treeNode->initialize(m_renderShaderManager, m_materialManager, m_modelManager);

      TreeNode *sibling = parent->getFirstChild();
      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
      treeNode->setNextSibling(sibling);

      return treeNode;
    }
	}
}