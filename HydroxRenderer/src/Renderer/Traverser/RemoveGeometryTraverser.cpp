#include "Renderer/Traverser/RemoveGeometryTraverser.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
	namespace renderer
	{
    RemoveGeometryTraverser::RemoveGeometryTraverser(util::SingletonManager *singletonManager, std::list<IRenderNode*>& renderNodes, xBar::StaticGeometryContainer& geometryContainer) : 
      m_renderNodes(renderNodes), 
      m_geometryContainer(geometryContainer)
    {
      m_modelManager = singletonManager->getService<ModelManager>();
      m_materialManager = singletonManager->getService<MaterialManager>();

      m_vertexDeclaration = m_modelManager->getObject(m_geometryContainer.getMeshHandle())->getVertexDeclarationFlags();

      Material *material = m_materialManager->getObject(m_geometryContainer.getMaterialHandle());
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

    RemoveGeometryTraverser::~RemoveGeometryTraverser()
    {
    }

    void RemoveGeometryTraverser::doTraverseDown(TreeNode* treeNode)
    {
      while(treeNode != nullptr)
      {
        if(treeNode->preTraverse(this))
        {
          doTraverseDown(treeNode->getFirstChild());
        }

        TreeNode *node = treeNode->getNextSibling();
        treeNode->postTraverse(this);
        treeNode = node;

        if(m_stopTraversal)
        {
          return;
        }
      }
    }

    bool RemoveGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return treeNode->isMesh(m_vertexDeclaration);
    }

    void RemoveGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        deleteNode(treeNode);
      }
    }

    bool RemoveGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      return treeNode->isShader(m_shaderHandle);
    }

    void RemoveGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        deleteNode(treeNode);
      }
    }

    bool RemoveGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      return treeNode->isTexture(m_textureHandles);
    }

    void RemoveGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        deleteNode(treeNode);
      }
    }

    bool RemoveGeometryTraverser::preTraverse(IRenderNode* treeNode)
    {
      return treeNode->removeGeometry(m_geometryContainer);
    }

    void RemoveGeometryTraverser::postTraverse(IRenderNode* treeNode)
    {
      if(treeNode->getInstanceNumber() == 0)
      {
        m_renderNodes.remove(treeNode);
        deleteNode(treeNode);
      }
    }

    void RemoveGeometryTraverser::deleteNode(TreeNode *treeNode)
    {
      TreeNode *currentRenderNode = treeNode->getParent();

      if(currentRenderNode != nullptr)
      {
        currentRenderNode = currentRenderNode->getFirstChild();
        TreeNode *oldRenderNode = currentRenderNode;

        while(currentRenderNode != nullptr)
        {
          if(currentRenderNode == treeNode)
          {
            if(oldRenderNode != currentRenderNode)
            {
              oldRenderNode->setNextSibling(currentRenderNode->getNextSibling());
            }
            else
            {
              currentRenderNode->getParent()->setFirstChild(currentRenderNode->getNextSibling());
            }

            delete treeNode;
            treeNode = nullptr;

            return;
          }

          oldRenderNode = currentRenderNode;
          currentRenderNode = currentRenderNode->getNextSibling();
        }
      }
      else
      {
        delete treeNode;
        treeNode = nullptr;
      }
    }
	}
}