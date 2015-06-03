#include "Renderer/Traverser/RemoveGeometryTraverser.h"

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
    RemoveGeometryTraverser::RemoveGeometryTraverser()
    {
      
    }

    RemoveGeometryTraverser::~RemoveGeometryTraverser()
    {
    }

    void RemoveGeometryTraverser::removeGeometry(util::SharedPointer<TreeNode>treeNode, util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager)
    {
      m_geometryContainer = geometryContainer;

      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();
    }

    void RemoveGeometryTraverser::doTraverseDown(util::SharedPointer<TreeNode> treeNode)
    {
      while(treeNode)
      {
        if(treeNode->preTraverse(this))
        {
          doTraverseDown(treeNode->getFirstChild());
        }

        util::SharedPointer<TreeNode> node = treeNode->getNextSibling();
        treeNode->postTraverse(this);
        treeNode = node;

        if(m_stopTraversal)
        {
          return;
        }
      }
    }

    bool RemoveGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return treeNode->isMesh(m_meshVertexDeclaration);
    }

    void RemoveGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      if(!treeNode->getFirstChild())
      {
        treeNode->setFirstChild(util::SharedPointer<TreeNode>());
      }
    }

    bool RemoveGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      return treeNode->isShader(m_shaderHandle);
    }

    void RemoveGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      if(!treeNode->getFirstChild())
      {
        treeNode->setFirstChild(util::SharedPointer<TreeNode>());
      }
    }

    bool RemoveGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      return treeNode->isTexture(m_textureHandles);
    }

    void RemoveGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
      if(!treeNode->getFirstChild())
      {
        treeNode->setFirstChild(util::SharedPointer<TreeNode>());
      }
    }

    //void RemoveGeometryTraverser::deleteNode(TreeNode *treeNode)
    //{
    //  TreeNode *currentRenderNode = treeNode;

    //  if(currentRenderNode != nullptr)
    //  {
    //    currentRenderNode = currentRenderNode->getFirstChild();
    //    util::SharedPointer<TreeNode> oldRenderNode = currentRenderNode;

    //    while(currentRenderNode != nullptr)
    //    {
    //      if(currentRenderNode == treeNode)
    //      {
    //        if(oldRenderNode != currentRenderNode)
    //        {
    //          oldRenderNode->setNextSibling(currentRenderNode->getNextSibling());
    //        }
    //        else
    //        {
    //          currentRenderNode->getParent()->setFirstChild(currentRenderNode->getNextSibling());
    //        }

    //        treeNode = util::SharedPointer<TreeNode>();

    //        return;
    //      }

    //      oldRenderNode = currentRenderNode;
    //      currentRenderNode = currentRenderNode->getNextSibling();
    //    }
    //  }
    //  else
    //  {
    //    treeNode = util::SharedPointer<TreeNode>();
    //  }
    //}
  }
}