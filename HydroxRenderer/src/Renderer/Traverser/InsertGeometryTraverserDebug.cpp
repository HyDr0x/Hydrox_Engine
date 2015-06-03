#include "Renderer/Traverser/InsertGeometryTraverserDebug.h"

#include <DataBase/Mesh.h>
#include <DataBase/ResourceManager.hpp>

#include <Shader/ShaderContainer.h>

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
    InsertGeometryTraverserDebug::InsertGeometryTraverserDebug()
    {
    }

    InsertGeometryTraverserDebug::~InsertGeometryTraverserDebug()
    {
    }

    void InsertGeometryTraverserDebug::insertGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_geometryContainer = geometryContainer;
      m_inserted = false;

      m_singletonManager = singletonManager;
      m_modelManager = m_singletonManager->getService<db::ModelManager>();
      m_materialManager = m_singletonManager->getService<db::MaterialManager>();
      m_shaderContainer = m_singletonManager->getService<sh::ShaderContainer>();

      db::Mesh *mesh = m_modelManager->getObject(geometryContainer->getMeshHandle());
      db::Material *material = m_materialManager->getObject(geometryContainer->getMaterialHandle());

      m_meshVertexDeclaration = mesh->getVertexDeclarationFlags();
      m_primitiveType = mesh->getPrimitiveType();
      m_vertexStride = mesh->getVertexStride();
      m_nodeType = m_geometryContainer->getNodeType();

      if(mesh->getIndexCount())
      {
        m_nodeType |= util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE);
      }
      else
      {
        m_nodeType |= util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::NONINDEXEDNODE);
      }

      m_shaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::RENDERDEBUG, sh::ShaderSlotFlags(m_meshVertexDeclaration.toInt()));

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

    bool InsertGeometryTraverserDebug::preTraverse(RenderNode * treeNode)
    {
      m_stopTraversal = m_inserted = treeNode->getRenderGroup()->insertGeometry(m_geometryContainer);

      if(!m_inserted && !treeNode->getNextSibling())
      {
        createNewSibling(treeNode);
      }

      return false;
    }

    void InsertGeometryTraverserDebug::createNewChildNode(VertexDeclarationNode * parent)
    {
      m_createdRenderGroup = RenderNodeFactory::createRenderNode(m_nodeType, m_primitiveType, m_vertexStride, m_singletonManager);
      util::SharedPointer<RenderNode> treeNode = util::SharedPointer<RenderNode>(new RenderNode(m_createdRenderGroup));

      parent->setFirstChild(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverserDebug::createNewSibling(RenderNode * sibling)
    {
      m_createdRenderGroup = RenderNodeFactory::createRenderNode(m_nodeType, m_primitiveType, m_vertexStride, m_singletonManager);
      util::SharedPointer<RenderNode> treeNode = util::SharedPointer<RenderNode>(new RenderNode(m_createdRenderGroup));

      sibling->setNextSibling(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(sibling->getParent());
    }
  }
}