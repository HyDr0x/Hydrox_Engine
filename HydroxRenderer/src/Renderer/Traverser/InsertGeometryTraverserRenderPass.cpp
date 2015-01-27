#include "Renderer/Traverser/InsertGeometryTraverserRenderPass.h"

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
    InsertGeometryTraverserRenderPass::InsertGeometryTraverserRenderPass(const xBar::IGeometryContainer& geometryContainer, util::SingletonManager *singletonManager) :
      InsertGeometryTraverser(geometryContainer, singletonManager),
      m_singletonManager(singletonManager),
      m_createdRenderGroup(util::SharedPointer<IRenderGroup>())
    {
      db::Mesh *mesh = m_modelManager->getObject(geometryContainer.getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer.getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();
      m_primitiveType = mesh->getPrimitiveType();
      m_vertexStride = mesh->getVertexStride();
      m_nodeType = m_geometryContainer.getNodeType();

      if(mesh->getIndexCount())
      {
        m_nodeType |= util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE);
      }
      else
      {
        m_nodeType |= util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::NONINDEXEDNODE);
      }

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      m_shaderHandle = m_renderShaderContainer->getRenderShader(singletonManager, 0, m_meshVertexDeclaration);

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

    InsertGeometryTraverserRenderPass::~InsertGeometryTraverserRenderPass()
    {
    }

    bool InsertGeometryTraverserRenderPass::preTraverse(RenderNode* treeNode)
    {
      m_stopTraversal = m_inserted = treeNode->getRenderGroup()->insertGeometry(m_geometryContainer);

      if(!m_inserted && treeNode->getNextSibling() == nullptr)
      {
        createNewSibling(treeNode);
      }

      return false;
    }

    util::SharedPointer<IRenderGroup> InsertGeometryTraverserRenderPass::getCreatedRenderNode() const
    {
      return m_createdRenderGroup;
    }

    void InsertGeometryTraverserRenderPass::createNewChildNode(VertexDeclarationNode* parent)
    {
      m_createdRenderGroup = RenderNodeFactory::createRenderNode(m_nodeType, m_primitiveType, m_vertexStride, m_singletonManager);
      RenderNode *treeNode = new RenderNode(m_createdRenderGroup);

      parent->setFirstChild(treeNode);
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverserRenderPass::createNewSibling(RenderNode* sibling)
    {
      m_createdRenderGroup = RenderNodeFactory::createRenderNode(m_nodeType, m_primitiveType, m_vertexStride, m_singletonManager);
      RenderNode *treeNode = new RenderNode(m_createdRenderGroup);

      sibling->setNextSibling(treeNode);
      treeNode->setParent(sibling->getParent());
    }
  }
}