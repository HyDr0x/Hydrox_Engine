#include "Renderer/Traverser/InsertGeometryTraverserReflectiveShadowPass.h"

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
    InsertGeometryTraverserReflectiveShadowPass::InsertGeometryTraverserReflectiveShadowPass()
    {
    }

    InsertGeometryTraverserReflectiveShadowPass::~InsertGeometryTraverserReflectiveShadowPass()
    {
    }

    void InsertGeometryTraverserReflectiveShadowPass::insertGeometry(TreeNode *rootNode, util::SharedPointer<IRenderGroup> sharedRenderGroup, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_sharedRenderGroup = sharedRenderGroup;
      m_geometryContainer = geometryContainer;
      m_inserted = false;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
      m_renderShaderContainer = singletonManager->getService<db::ShaderContainer>();

      db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer->getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, db::ShaderContainer::REFLECTIVESHADOW, m_meshVertexDeclaration);

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

    bool InsertGeometryTraverserReflectiveShadowPass::preTraverse(RenderNode* treeNode)
    {
      if(!m_stopTraversal && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }

      return false;
    }

    void InsertGeometryTraverserReflectiveShadowPass::createNewChildNode(VertexDeclarationNode* parent)
    {
      m_stopTraversal = true;

      RenderNode *treeNode = new RenderNode(m_sharedRenderGroup);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverserReflectiveShadowPass::createNewSibling(RenderNode* sibling)
    {
      m_stopTraversal = true;

      RenderNode *treeNode = new RenderNode(m_sharedRenderGroup);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}