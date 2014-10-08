#include "Renderer/Traverser/RemoveShadowGeometryTraverser.h"

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
    RemoveShadowGeometryTraverser::RemoveShadowGeometryTraverser(
      util::SingletonManager *singletonManager, 
      const xBar::IGeometryContainer& geometryContainer,
      util::ResourceHandle staticShadowMapGenerationShaderHandle,
      util::ResourceHandle skinnedShadowMapGenerationShaderHandle) :
      m_geometryContainer(geometryContainer)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();

      m_vertexDeclaration = m_modelManager->getObject(m_geometryContainer.getMeshHandle())->getVertexDeclarationFlags();

      if(geometryContainer.getNodeType() == util::Flags<xBar::RenderNodeType>(xBar::SKINNEDNODE))
      {
        m_shaderHandle = skinnedShadowMapGenerationShaderHandle;
      }
      else
      {
        m_shaderHandle = staticShadowMapGenerationShaderHandle;
      }
    }

    RemoveShadowGeometryTraverser::~RemoveShadowGeometryTraverser()
    {
    }

    void RemoveShadowGeometryTraverser::doTraverseDown(TreeNode* treeNode)
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

    bool RemoveShadowGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return treeNode->isMesh(m_vertexDeclaration);
    }

    void RemoveShadowGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        deleteNode(treeNode);
      }
    }

    bool RemoveShadowGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      return treeNode->isShader(m_shaderHandle);
    }

    void RemoveShadowGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        deleteNode(treeNode);
      }
    }

    bool RemoveShadowGeometryTraverser::preTraverse(RenderNode* treeNode)
    {
      return false;
    }

    void RemoveShadowGeometryTraverser::postTraverse(RenderNode* treeNode)
    {
      if(treeNode->getRenderGroup()->getInstanceNumber() == 0)
      {
        deleteNode(treeNode);
        m_stopTraversal = true;
      }
    }

    void RemoveShadowGeometryTraverser::deleteNode(TreeNode *treeNode)
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