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
    InsertGeometryTraverser::InsertGeometryTraverser(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle) : 
      m_maxMaterials(maxMaterials),
      m_maxGeometry(maxGeometry),
      m_maxBones(maxBones),
      m_cullingShaderHandle(cullingShaderHandle)
    {
      m_node = nullptr;
      m_parentNode = nullptr;

      m_modelManager = singletonManager->getService<ModelManager>();
      m_materialManager = singletonManager->getService<MaterialManager>();
      m_computeShaderManager = singletonManager->getService<ComputeShaderManager>();
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();
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
      return treeNode->insertGeometry(m_node);
    }

    void InsertGeometryTraverser::postTraverse(StaticRenderNode* treeNode)
    {
    }

    bool InsertGeometryTraverser::preTraverse(AnimatedRenderNode* treeNode)
    {
      return treeNode->insertGeometry(m_node);
    }

    void InsertGeometryTraverser::postTraverse(AnimatedRenderNode* treeNode)
    {
    }

    TreeNode* InsertGeometryTraverser::createNewNode(TreeNode* parent)
    {
      return nullptr;
    }

    TreeNode* InsertGeometryTraverser::createNewNode(TextureNode* parent)
    {
      VertexDeclarationNode *treeNode = new VertexDeclarationNode();
      treeNode->initialize(m_vertexDeclaration);

      TreeNode *sibling = parent->getFirstChild();
      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
      treeNode->setNextSibling(sibling);

      return treeNode;
    }

    TreeNode* InsertGeometryTraverser::createNewNode(GroupNode* parent)
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

    TreeNode* InsertGeometryTraverser::createNewNode(VertexDeclarationNode* parent)
    {
      RenderNode *treeNode;
      if(dynamic_cast<sg::AnimatedGeoNode*>(m_node) != nullptr)
      {
        treeNode = new AnimatedRenderNode(m_maxMaterials, m_maxGeometry, m_maxBones);
      }
      else
      {
        treeNode = new StaticRenderNode(m_maxMaterials, m_maxGeometry);
      }

      treeNode->initialize(m_materialManager, m_modelManager, m_cullingShaderHandle);

      TreeNode *sibling = parent->getFirstChild();
      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
      treeNode->setNextSibling(sibling);

      return treeNode;
    }
	}
}