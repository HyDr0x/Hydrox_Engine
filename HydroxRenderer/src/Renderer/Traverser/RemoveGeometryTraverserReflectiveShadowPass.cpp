#include "Renderer/Traverser/RemoveGeometryTraverserReflectiveShadowPass.h"

#include <XBar/IGeometryContainer.h>

#include <Shader/ShaderContainer.h>

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
    RemoveGeometryTraverserReflectiveShadowPass::RemoveGeometryTraverserReflectiveShadowPass()
    {
    }

    RemoveGeometryTraverserReflectiveShadowPass::~RemoveGeometryTraverserReflectiveShadowPass()
    {
    }

    void RemoveGeometryTraverserReflectiveShadowPass::removeGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_geometryContainer = geometryContainer;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer->getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      m_shaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::REFLECTIVESHADOW, sh::ShaderSlotFlags(m_meshVertexDeclaration.toInt()));

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

      if(m_shaderHandle)
      {
        m_shaderVertexDeclaration = m_shaderContainer->getRenderShader(m_shaderHandle).getVertexDeclaration();

        doTraverse(treeNode);
      }
    }

    bool RemoveGeometryTraverserReflectiveShadowPass::preTraverse(RenderNode * treeNode)
    {
      return false;
    }

    void RemoveGeometryTraverserReflectiveShadowPass::postTraverse(RenderNode * treeNode)
    {
      if(treeNode->getRenderGroup()->getInstanceNumber() == 0)
      {
        m_stopTraversal = true;
      }
    }
  }
}