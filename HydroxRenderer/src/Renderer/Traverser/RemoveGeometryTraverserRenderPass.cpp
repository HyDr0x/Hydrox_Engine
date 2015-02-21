#include "Renderer/Traverser/RemoveGeometryTraverserRenderPass.h"

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
    RemoveGeometryTraverserRenderPass::RemoveGeometryTraverserRenderPass()
    {
    }

    RemoveGeometryTraverserRenderPass::~RemoveGeometryTraverserRenderPass()
    {
    }

    void RemoveGeometryTraverserRenderPass::removeGeometry(TreeNode *rootNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_geometryContainer = geometryContainer;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
      m_renderShaderContainer = singletonManager->getService<db::ShaderContainer>();

      db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer->getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, db::ShaderContainer::GBUFFER, m_meshVertexDeclaration);

      if(m_shaderHandle)
      {
        m_shaderVertexDeclaration = m_renderShaderManager->getObject(m_shaderHandle)->getVertexDeclaration();
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

        doTraverse(rootNode);
      }
    }

    bool RemoveGeometryTraverserRenderPass::preTraverse(RenderNode* treeNode)
    {
      return false;
    }

    void RemoveGeometryTraverserRenderPass::postTraverse(RenderNode* treeNode)
    {
      treeNode->getRenderGroup()->removeGeometry(m_geometryContainer);
      if(treeNode->getRenderGroup()->getInstanceNumber() == 0)
      {
        deleteNode(treeNode);
        m_stopTraversal = true;
      }
    }
  }
}