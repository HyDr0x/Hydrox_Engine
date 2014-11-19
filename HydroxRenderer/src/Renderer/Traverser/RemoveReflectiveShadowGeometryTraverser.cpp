#include "Renderer/Traverser/RemoveReflectiveShadowGeometryTraverser.h"

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"

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
    RemoveReflectiveShadowGeometryTraverser::RemoveReflectiveShadowGeometryTraverser(
      util::SingletonManager *singletonManager, 
      const xBar::IGeometryContainer& geometryContainer,
      RenderShaderContainer *container) :
      m_geometryContainer(geometryContainer)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();

      m_vertexDeclaration = m_modelManager->getObject(m_geometryContainer.getMeshHandle())->getVertexDeclarationFlags();

      db::Mesh *mesh = singletonManager->getService<db::ModelManager>()->getObject(geometryContainer.getMeshHandle());

      if(geometryContainer.getNodeType() == util::Flags<xBar::RenderNodeType>(xBar::SKINNEDNODE))
      {
        if(mesh->getVertexDeclarationFlags() & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_BINORMAL))
        {
          if(mesh->getVertexDeclarationFlags() & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_COLOR))
          {
            m_shaderHandle = container->skinnedNormalReflectiveShadowMapGenerationVertexColorShaderHandle;
          }
          else
          {
            m_shaderHandle = container->skinnedNormalReflectiveShadowMapGenerationShaderHandle;
          }
        }
        else
        {
          if(mesh->getVertexDeclarationFlags() & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_COLOR))
          {
            m_shaderHandle = container->skinnedReflectiveShadowMapGenerationVertexColorShaderHandle;
          }
          else
          {
            m_shaderHandle = container->skinnedReflectiveShadowMapGenerationShaderHandle;
          }
        }
      }
      else
      {
        if(mesh->getVertexDeclarationFlags() & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_BINORMAL))
        {
          if(mesh->getVertexDeclarationFlags() & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_COLOR))
          {
            m_shaderHandle = container->staticNormalReflectiveShadowMapGenerationVertexColorShaderHandle;
          }
          else
          {
            m_shaderHandle = container->staticNormalReflectiveShadowMapGenerationShaderHandle;
          }
        }
        else
        {
          if(mesh->getVertexDeclarationFlags() & db::Mesh::vertexDeclarationFlag(db::Mesh::MODEL_COLOR))
          {
            m_shaderHandle = container->staticReflectiveShadowMapGenerationVertexColorShaderHandle;
          }
          else
          {
            m_shaderHandle = container->staticReflectiveShadowMapGenerationShaderHandle;
          }
        }
      }

      db::Material *material = singletonManager->getService<db::MaterialManager>()->getObject(geometryContainer.getMaterialHandle());

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

    RemoveReflectiveShadowGeometryTraverser::~RemoveReflectiveShadowGeometryTraverser()
    {
    }

    void RemoveReflectiveShadowGeometryTraverser::doTraverseDown(TreeNode* treeNode)
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

    bool RemoveReflectiveShadowGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      return treeNode->isShader(m_shaderHandle);
    }

    void RemoveReflectiveShadowGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        deleteNode(treeNode);
      }
    }

    bool RemoveReflectiveShadowGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      return treeNode->isTexture(m_textureHandles);
    }

    void RemoveReflectiveShadowGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        deleteNode(treeNode);
      }
    }

    bool RemoveReflectiveShadowGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return treeNode->isMesh(m_vertexDeclaration);
    }

    void RemoveReflectiveShadowGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      if(treeNode->getFirstChild() == nullptr)
      {
        deleteNode(treeNode);
      }
    }

    bool RemoveReflectiveShadowGeometryTraverser::preTraverse(RenderNode* treeNode)
    {
      return false;
    }

    void RemoveReflectiveShadowGeometryTraverser::postTraverse(RenderNode* treeNode)
    {
      if(treeNode->getRenderGroup()->getInstanceNumber() == 0)
      {
        deleteNode(treeNode);
        m_stopTraversal = true;
      }
    }

    void RemoveReflectiveShadowGeometryTraverser::deleteNode(TreeNode *treeNode)
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