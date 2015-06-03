#include "Renderer/Traverser/RenderGeometryTraverserDebug.h"

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
    RenderGeometryTraverserDebug::RenderGeometryTraverserDebug()
    {
    }

    RenderGeometryTraverserDebug::~RenderGeometryTraverserDebug()
    {
    }

    void RenderGeometryTraverserDebug::initialize(util::SingletonManager *singletonManager, std::vector<util::SharedPointer<SamplerObject>> samplerObjects)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_textureManager = singletonManager->getService<db::TextureManager>();
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      m_samplerObjects = samplerObjects;
    }

    bool RenderGeometryTraverserDebug::preTraverse(GroupNode * treeNode)
    {
      return true;
    }

    bool RenderGeometryTraverserDebug::preTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->setVertexArray();

      return true;
    }

    void RenderGeometryTraverserDebug::postTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->unsetVertexArray();
    }

    bool RenderGeometryTraverserDebug::preTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useShader();

      return true;
    }

    void RenderGeometryTraverserDebug::postTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useNoShader();
    }

    bool RenderGeometryTraverserDebug::preTraverse(TextureNode * treeNode)
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

    void RenderGeometryTraverserDebug::postTraverse(TextureNode * treeNode)
    {
      const std::vector< std::vector<util::ResourceHandle> >& textureHandles = treeNode->getTextureHandles();

      GLuint slotOffset = 0;

      for(unsigned int i = 0; i < textureHandles.size(); i++)
      {
        for(unsigned int j = 0; j < textureHandles[i].size(); j++)
        {
          db::Texture2D *texture = m_textureManager->getObject(textureHandles[i][j]);
          texture->unsetTexture(slotOffset + j);
          m_samplerObjects[i]->unbindSampler(slotOffset + j);
        }

        slotOffset += static_cast<GLuint>(textureHandles[i].size());
      }
    }

    bool RenderGeometryTraverserDebug::preTraverse(RenderNode * treeNode)
    {
      treeNode->getRenderGroup()->rasterizeGeometry();

      return true;
    }

    void RenderGeometryTraverserDebug::postTraverse(RenderNode * treeNode)
    {
    }
  }
}