#include "Renderer/Traverser/RenderReflectiveShadowGeometryTraverser.h"

#include <Shader/ShaderContainer.h>

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
    RenderReflectiveShadowGeometryTraverser::RenderReflectiveShadowGeometryTraverser()
    {
    }

    RenderReflectiveShadowGeometryTraverser::~RenderReflectiveShadowGeometryTraverser()
    {
    }

    void RenderReflectiveShadowGeometryTraverser::initialize(util::SingletonManager *singletonManager)
    {
      m_textureManager = singletonManager->getService<db::TextureManager>();
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_options = singletonManager->getService<RenderOptions>();
    }

    void RenderReflectiveShadowGeometryTraverser::setViewProjectionIndex(int viewProjectionIndex)
    {
      m_viewProjectionIndex = viewProjectionIndex;
    }

    bool RenderReflectiveShadowGeometryTraverser::preTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->setVertexArray();

      return true;
    }

    void RenderReflectiveShadowGeometryTraverser::postTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->unsetVertexArray();
    }

    bool RenderReflectiveShadowGeometryTraverser::preTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useShader();

      sh::RenderShader::setUniform(2, GL_INT, &m_viewProjectionIndex);
      sh::RenderShader::setUniform(3, GL_UNSIGNED_INT, &m_options->reflectiveShadowMapWidth);

      return true;
    }

    void RenderReflectiveShadowGeometryTraverser::postTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useNoShader();
    }

    bool RenderReflectiveShadowGeometryTraverser::preTraverse(TextureNode * treeNode)
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

    void RenderReflectiveShadowGeometryTraverser::postTraverse(TextureNode * treeNode)
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

    bool RenderReflectiveShadowGeometryTraverser::preTraverse(RenderNode * treeNode)
    {
      treeNode->getRenderGroup()->rasterizeReflectiveShadowGeometry();

      return true;
    }

    void RenderReflectiveShadowGeometryTraverser::postTraverse(RenderNode * treeNode)
    {
    }
  }
}