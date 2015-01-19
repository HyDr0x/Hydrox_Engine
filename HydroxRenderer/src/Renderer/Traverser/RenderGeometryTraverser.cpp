#include "Renderer/Traverser/RenderGeometryTraverser.h"

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
    RenderGeometryTraverser::RenderGeometryTraverser()
    {
    }

    RenderGeometryTraverser::~RenderGeometryTraverser()
    {
    }

    void RenderGeometryTraverser::initialize(util::SingletonManager *singletonManager, std::vector<util::SharedPointer<SamplerObject>> samplerObjects)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_textureManager = singletonManager->getService<db::TextureManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();

      m_samplerObjects = samplerObjects;
    }

    bool RenderGeometryTraverser::preTraverse(GroupNode* treeNode)
    {
      return true;
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
      db::RenderShader *shader = m_renderShaderManager->getObject(treeNode->getShaderHandle());

      shader->useShader();

      return true;
    }

    void RenderGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      db::RenderShader *shader = m_renderShaderManager->getObject(treeNode->getShaderHandle());

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
          m_samplerObjects[i]->bindSampler(slotOffset + j);
          db::Texture2D *texture = m_textureManager->getObject(textureHandles[i][j]);
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
          db::Texture2D *texture = m_textureManager->getObject(textureHandles[i][j]);
          texture->unsetTexture();
          m_samplerObjects[i]->unbindSampler();
        }
      }
    }

    bool RenderGeometryTraverser::preTraverse(RenderNode* treeNode)
    {
      treeNode->getRenderGroup()->rasterizeGeometry();

      return true;
    }

    void RenderGeometryTraverser::postTraverse(RenderNode* treeNode)
    {
    }
  }
}