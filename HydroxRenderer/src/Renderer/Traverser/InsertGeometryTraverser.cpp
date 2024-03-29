#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    InsertGeometryTraverser::InsertGeometryTraverser(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, std::list<IRenderNode*>& renderNodes, xBar::StaticGeometryContainer& geometryContainer, bool skinned, bool instanced) : 
      m_instanced(instanced),
      m_maxMaterials(maxMaterials),
      m_maxGeometry(maxGeometry),
      m_maxBones(maxBones),
      m_singletonManager(singletonManager),
      m_renderNodes(renderNodes),
      m_geometryContainer(geometryContainer),
      m_inserted(false)
    {
      Mesh* mesh = m_singletonManager->getService<ModelManager>()->getObject(m_geometryContainer.getMeshHandle());
      m_vertexDeclaration = mesh->getVertexDeclarationFlags();
      m_primitiveType = mesh->getPrimitiveType();
      m_vertexStride = mesh->getVertexStride();

      if(mesh->getIndexCount() > 0)
      {
        if(skinned)
        {
          m_nodeType = SkinnedIndexedNode;
        }
        else
        {
          m_nodeType = NonSkinnedIndexedNode;
        }
      }
      else
      {
        if(skinned)
        {
          m_nodeType = SkinnedNonIndexedNode;
        }
        else
        {
          m_nodeType = NonSkinnedNonIndexedNode;
        }
      }

      Material *material = m_singletonManager->getService<MaterialManager>()->getObject(m_geometryContainer.getMaterialHandle());
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
      IRenderNode *treeNode = RenderNodeFactory::createRenderNode(m_nodeType, m_instanced, m_maxMaterials, m_maxGeometry, m_maxBones, m_primitiveType, m_vertexStride, m_singletonManager);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);

      m_renderNodes.push_back(treeNode);
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
      IRenderNode *treeNode = RenderNodeFactory::createRenderNode(m_nodeType, m_instanced, m_maxMaterials, m_maxGeometry, m_maxBones, m_primitiveType, m_vertexStride, m_singletonManager);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());

      m_renderNodes.push_back(treeNode);
    }
	}
}