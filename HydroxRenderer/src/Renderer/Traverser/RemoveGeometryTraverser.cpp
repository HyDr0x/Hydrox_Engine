#include "Renderer/Traverser/RemoveGeometryTraverser.h"

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    RemoveGeometryTraverser::RemoveGeometryTraverser()
    {
      
    }

    RemoveGeometryTraverser::~RemoveGeometryTraverser()
    {
    }

    void RemoveGeometryTraverser::removeGeometry(TreeNode *treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_geometryContainer = geometryContainer;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
      m_renderShaderContainer = singletonManager->getService<db::ShaderContainer>();
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
      return treeNode->isMesh(m_meshVertexDeclaration);
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