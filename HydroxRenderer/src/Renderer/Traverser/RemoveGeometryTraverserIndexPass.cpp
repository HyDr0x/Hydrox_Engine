#include "Renderer/Traverser/RemoveGeometryTraverserIndexPass.h"

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
    RemoveGeometryTraverserIndexPass::RemoveGeometryTraverserIndexPass(
      util::SingletonManager *singletonManager, 
      const xBar::IGeometryContainer& geometryContainer,
      util::ResourceHandle staticIndexGenerationShaderHandle,
      util::ResourceHandle skinnedIndexGenerationShaderHandle) :
      RemoveGeometryTraverser(singletonManager, geometryContainer)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();

      m_vertexDeclaration = m_modelManager->getObject(m_geometryContainer.getMeshHandle())->getVertexDeclarationFlags();

      db::Mesh *mesh = singletonManager->getService<db::ModelManager>()->getObject(geometryContainer.getMeshHandle());

      if(geometryContainer.getNodeType() == util::Flags<xBar::RenderNodeType>(xBar::SKINNEDNODE))
      {
        m_shaderHandle = staticIndexGenerationShaderHandle;
        
      }
      else
      {
        m_shaderHandle = skinnedIndexGenerationShaderHandle;
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

    RemoveGeometryTraverserIndexPass::~RemoveGeometryTraverserIndexPass()
    {
    }

    bool RemoveGeometryTraverserIndexPass::preTraverse(RenderNode* treeNode)
    {
      return false;
    }

    void RemoveGeometryTraverserIndexPass::postTraverse(RenderNode* treeNode)
    {
      if(treeNode->getRenderGroup()->getInstanceNumber() == 0)
      {
        deleteNode(treeNode);
        m_stopTraversal = true;
      }
    }
  }
}