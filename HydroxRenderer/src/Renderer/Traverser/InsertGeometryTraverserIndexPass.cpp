#include "Renderer/Traverser/InsertGeometryTraverserIndexPass.h"

#include <DataBase/Mesh.h>
#include <DataBase/ShaderContainer.h>
#include <DataBase/ResourceManager.hpp>

#include <XBar/IGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    InsertGeometryTraverserIndexPass::InsertGeometryTraverserIndexPass(
      util::SharedPointer<IRenderGroup> sharedRenderGroup,
      const xBar::IGeometryContainer& geometryContainer,
      util::SingletonManager *singletonManager) :
      InsertGeometryTraverser(geometryContainer, singletonManager),
      m_sharedRenderGroup(sharedRenderGroup)
    {
      db::Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer.getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, 1, m_meshVertexDeclaration);

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
    }

    InsertGeometryTraverserIndexPass::~InsertGeometryTraverserIndexPass()
    {
    }

    bool InsertGeometryTraverserIndexPass::preTraverse(RenderNode* treeNode)
    {
      if(!m_stopTraversal && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }

      return false;
    }

    void InsertGeometryTraverserIndexPass::createNewChildNode(VertexDeclarationNode* parent)
    {
      m_stopTraversal = true;

      RenderNode *treeNode = new RenderNode(m_sharedRenderGroup);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverserIndexPass::createNewSibling(RenderNode* sibling)
    {
      m_stopTraversal = true;

      RenderNode *treeNode = new RenderNode(m_sharedRenderGroup);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}