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
    RemoveGeometryTraverserIndexPass::RemoveGeometryTraverserIndexPass()
    {
    }

    RemoveGeometryTraverserIndexPass::~RemoveGeometryTraverserIndexPass()
    {
    }

    void RemoveGeometryTraverserIndexPass::removeGeometry(TreeNode *treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_geometryContainer = geometryContainer;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
      m_renderShaderContainer = singletonManager->getService<db::ShaderContainer>();

      db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer->getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      switch(mesh->getPrimitiveType())
      {
      case GL_POINTS:
        m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, db::ShaderContainer::POINTINDEX, m_meshVertexDeclaration);
        break;
      case GL_LINES:
        m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, db::ShaderContainer::LINEINDEX, m_meshVertexDeclaration);
        break;
      case GL_TRIANGLES:
      default:
        m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, db::ShaderContainer::TRIANGLEINDEX, m_meshVertexDeclaration);
        break;
      }

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
        m_shaderVertexDeclaration = m_renderShaderManager->getObject(m_shaderHandle)->getVertexDeclaration();

        doTraverse(treeNode);
      }
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