#include "Renderer/Traverser/RenderGeometryTraverser.h"

#include "Renderer/TreeNodes/TreeNode.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
  namespace renderer
  {
    RenderGeometryTraverser::RenderGeometryTraverser(util::SingletonManager *singletonManager)
    {
      m_modelManager = singletonManager->getService<ModelManager>();
      m_materialManager = singletonManager->getService<MaterialManager>();
      m_textureManager = singletonManager->getService<TextureManager>();
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();
    }

    RenderGeometryTraverser::~RenderGeometryTraverser()
    {
    }

    bool RenderGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      treeNode->setVertexArray();

      return true;
    }

    void RenderGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      treeNode->unsetVertexArray();
    }

    bool RenderGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      RenderShader *shader = m_renderShaderManager->getObject(treeNode->getShaderHandle());

      shader->useShader();

      return true;
    }

    void RenderGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      RenderShader *shader = m_renderShaderManager->getObject(treeNode->getShaderHandle());

      shader->useNoShader();
    }

    bool RenderGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      const std::vector< std::vector<util::ResourceHandle> >& textureHandles = treeNode->getTextureHandles();

      GLuint slotOffset = 0;

      for(unsigned int i = 0; i < textureHandles.size(); i++)
      {
        for(unsigned int j = 0; j < textureHandles[i].size(); j++)
        {
          Texture2D *texture = m_textureManager->getObject(textureHandles[i][j]);
          texture->setTexParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
          texture->setTexture(static_cast<GLint>(slotOffset + j), slotOffset + j);
        }

        slotOffset += static_cast<GLuint>(textureHandles[i].size());
      }

      return true;
    }

    void RenderGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
      const std::vector< std::vector<util::ResourceHandle> >& textureHandles = treeNode->getTextureHandles();

      for(unsigned int i = 0; i < textureHandles.size(); i++)
      {
        for(unsigned int j = 0; j < textureHandles[i].size(); j++)
        {
          Texture2D *texture = m_textureManager->getObject(textureHandles[i][j]);
          texture->unsetTexture();
        }
      }
    }

    bool RenderGeometryTraverser::preTraverse(IRenderNode* treeNode)
    {
      treeNode->rasterizeGeometry();

      return true;
    }

    void RenderGeometryTraverser::postTraverse(IRenderNode* treeNode)
    {
    }
  }
}