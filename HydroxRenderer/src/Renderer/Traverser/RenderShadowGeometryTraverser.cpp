#include "Renderer/Traverser/RenderShadowGeometryTraverser.h"

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
    RenderShadowGeometryTraverser::RenderShadowGeometryTraverser()
    {
    }

    RenderShadowGeometryTraverser::~RenderShadowGeometryTraverser()
    {
    }

    void RenderShadowGeometryTraverser::initialize(util::SingletonManager *singletonManager)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_textureManager = singletonManager->getService<db::TextureManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();

      m_options = singletonManager->getService<RenderOptions>();
    }

    void RenderShadowGeometryTraverser::setViewProjectionIndex(int viewProjectionIndex)
    {
      m_viewProjectionIndex = viewProjectionIndex;
    }

    bool RenderShadowGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      treeNode->setVertexArray();

      return true;
    }

    void RenderShadowGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      treeNode->unsetVertexArray();
    }

    bool RenderShadowGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      db::RenderShader *shader = m_renderShaderManager->getObject(treeNode->getShaderHandle());

      shader->useShader();

      unsigned int reflectiveShadoMapResolution = sqrt((m_options->shadowMapWidth * m_options->shadowMapWidth) / float(m_options->unusedLightIndirectNumber));

      db::RenderShader::setUniform(2, GL_INT, &m_viewProjectionIndex);
      db::RenderShader::setUniform(3, GL_UNSIGNED_INT, &reflectiveShadoMapResolution);

      return true;
    }

    void RenderShadowGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
      db::RenderShader *shader = m_renderShaderManager->getObject(treeNode->getShaderHandle());

      shader->useNoShader();
    }

    bool RenderShadowGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      const std::vector< std::vector<util::ResourceHandle> >& textureHandles = treeNode->getTextureHandles();

      GLuint slotOffset = 0;

      for(unsigned int i = 0; i < textureHandles.size(); i++)
      {
        for(unsigned int j = 0; j < textureHandles[i].size(); j++)
        {
          db::Texture2D *texture = m_textureManager->getObject(textureHandles[i][j]);
          texture->setTexture(static_cast<GLint>(slotOffset + j), slotOffset + j);
        }

        slotOffset += static_cast<GLuint>(textureHandles[i].size());
      }

      return true;
    }

    void RenderShadowGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
      const std::vector< std::vector<util::ResourceHandle> >& textureHandles = treeNode->getTextureHandles();

      GLuint slotOffset = 0;

      for(unsigned int i = 0; i < textureHandles.size(); i++)
      {
        for(unsigned int j = 0; j < textureHandles[i].size(); j++)
        {
          db::Texture2D *texture = m_textureManager->getObject(textureHandles[i][j]);
          texture->unsetTexture(slotOffset + j);
        }

        slotOffset += static_cast<GLuint>(textureHandles[i].size());
      }
    }

    bool RenderShadowGeometryTraverser::preTraverse(RenderNode* treeNode)
    {
      treeNode->getRenderGroup()->rasterizeShadowGeometry();

      return true;
    }

    void RenderShadowGeometryTraverser::postTraverse(RenderNode* treeNode)
    {
    }
  }
}