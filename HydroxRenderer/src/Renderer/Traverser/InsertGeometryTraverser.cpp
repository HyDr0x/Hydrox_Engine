#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include <DataBase/Mesh.h>

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
    InsertGeometryTraverser::InsertGeometryTraverser()
    {
    }

    InsertGeometryTraverser::~InsertGeometryTraverser()
    {
    }

    void InsertGeometryTraverser::insertGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_geometryContainer = geometryContainer;
      m_inserted = false;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderContainer = singletonManager->getService<sh::ShaderContainer>();
    }

    bool InsertGeometryTraverser::preTraverse(GroupNode * treeNode)
    {
      if(!treeNode->getFirstChild())
      {
        createNewChildNode(treeNode);
      }

      return true;
    }

    bool InsertGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      m_inserted = treeNode->isMesh(m_meshVertexDeclaration);

      if(m_inserted)
      {
        if(!treeNode->getFirstChild())
        {
          createNewChildNode(treeNode);
        }
      }
      else if(!treeNode->getNextSibling())
      {
        createNewSibling(treeNode);
      }

      return m_inserted;
    }

    bool InsertGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      m_inserted = treeNode->isShader(m_shaderHandle);

      if(m_inserted)
      {
        if(!treeNode->getFirstChild())
        {
          createNewChildNode(treeNode);
        }
      }
      else if(!treeNode->getNextSibling())
      {
        createNewSibling(treeNode);
      }

      return m_inserted;
    }

    bool InsertGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      m_inserted = treeNode->isTexture(m_textureHandles);

      if(m_inserted)
      {
        if(!treeNode->getFirstChild())
        {
          createNewChildNode(treeNode);
        }
      }
      else if(!treeNode->getNextSibling())
      {
        createNewSibling(treeNode);
      }

      return m_inserted;
    }

    void InsertGeometryTraverser::createNewChildNode(GroupNode * parent)
    {
      util::SharedPointer<ShaderNode> treeNode = util::SharedPointer<ShaderNode>(new ShaderNode());
      treeNode->initialize(m_shaderHandle);

      parent->setFirstChild(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverser::createNewChildNode(TextureNode* parent)
    {
      util::SharedPointer<VertexDeclarationNode> treeNode = util::SharedPointer<VertexDeclarationNode>(new VertexDeclarationNode());
      treeNode->initialize(m_shaderVertexDeclaration, m_meshVertexDeclaration);

      parent->setFirstChild(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverser::createNewChildNode(ShaderNode* parent)
    {
      util::SharedPointer<TextureNode> treeNode = util::SharedPointer<TextureNode>(new TextureNode());
      treeNode->initialize(m_textureHandles);

      parent->setFirstChild(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(parent);
    }

    void InsertGeometryTraverser::createNewSibling(TextureNode* sibling)
    {
      util::SharedPointer<TextureNode> treeNode = util::SharedPointer<TextureNode>(new TextureNode());
      treeNode->initialize(m_textureHandles);

      sibling->setNextSibling(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(sibling->getParent());
    }

    void InsertGeometryTraverser::createNewSibling(ShaderNode* sibling)
    {
      util::SharedPointer<ShaderNode> treeNode = util::SharedPointer<ShaderNode>(new ShaderNode());
      treeNode->initialize(m_shaderHandle);

      sibling->setNextSibling(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(sibling->getParent());
    }

    void InsertGeometryTraverser::createNewSibling(VertexDeclarationNode* sibling)
    {
      util::SharedPointer<VertexDeclarationNode> treeNode = util::SharedPointer<VertexDeclarationNode>(new VertexDeclarationNode());
      treeNode->initialize(m_shaderVertexDeclaration, m_meshVertexDeclaration);

      sibling->setNextSibling(treeNode.dynamic_pointer_cast<TreeNode>());
      treeNode->setParent(sibling->getParent());
    }
  }
}