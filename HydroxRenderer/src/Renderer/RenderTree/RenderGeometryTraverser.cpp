#include "Renderer/RenderTree/RenderGeometryTraverser.h"

#include "Renderer/RenderTree/TreeNode.h"

#include "Renderer/RenderTree/VertexDeclarationNode.h"
#include "Renderer/RenderTree/ShaderNode.h"
#include "Renderer/RenderTree/TextureNode.h"
#include "Renderer/RenderTree/StaticRenderNode.h"
#include "Renderer/RenderTree/AnimatedRenderNode.h"

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
      GLuint vertexDeclaration = treeNode->getVertexDeclaration();

      if(vertexDeclaration & Mesh::MODEL_POSITION)
      {
        glEnableVertexAttribArray(RenderShader::POSITION);
      }

      if(vertexDeclaration & Mesh::MODEL_TEXTURE)
      {
        glEnableVertexAttribArray(RenderShader::TEXTURE0);
      }

      if(vertexDeclaration & Mesh::MODEL_NORMAL)
      {
        glEnableVertexAttribArray(RenderShader::NORMAL);
      }

      if(vertexDeclaration & Mesh::MODEL_BINORMAL)
      {
        glEnableVertexAttribArray(RenderShader::BINORMAL);
      }

      if(vertexDeclaration & Mesh::MODEL_BONE_INDICES)
      {
        glEnableVertexAttribArray(RenderShader::BONEINDICES);
      }

      if(vertexDeclaration & Mesh::MODEL_BONE_WEIGHTS)
      {
        glEnableVertexAttribArray(RenderShader::BONEWEIGHTS);
      }

      if(vertexDeclaration & Mesh::MODEL_COLOR)
      {
        glEnableVertexAttribArray(RenderShader::COLOR);
      }

      return true;
    }

    void RenderGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      GLuint vertexDeclaration = treeNode->getVertexDeclaration();

      if(vertexDeclaration & Mesh::MODEL_POSITION)
      {
        glDisableVertexAttribArray(RenderShader::POSITION);
      }

      if(vertexDeclaration & Mesh::MODEL_TEXTURE)
      {
        glDisableVertexAttribArray(RenderShader::TEXTURE0);
      }

      if(vertexDeclaration & Mesh::MODEL_NORMAL)
      {
        glDisableVertexAttribArray(RenderShader::NORMAL);
      }

      if(vertexDeclaration & Mesh::MODEL_BINORMAL)
      {
        glDisableVertexAttribArray(RenderShader::BINORMAL);
      }

      if(vertexDeclaration & Mesh::MODEL_BONE_INDICES)
      {
        glDisableVertexAttribArray(RenderShader::BONEINDICES);
      }

      if(vertexDeclaration & Mesh::MODEL_BONE_WEIGHTS)
      {
        glDisableVertexAttribArray(RenderShader::BONEWEIGHTS);
      }

      if(vertexDeclaration & Mesh::MODEL_COLOR)
      {
        glDisableVertexAttribArray(RenderShader::COLOR);
      }
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
      std::vector< std::vector<util::ResourceHandle> >& textureHandles = treeNode->getTextureHandles();

      size_t slotOffset = 0;

      for(unsigned int j = 0; j < textureHandles[Material::DIFFUSETEX].size(); j++)
      {
        Texture *texture = m_textureManager->getObject(textureHandles[Material::DIFFUSETEX][j]);
        texture->setTexture(slotOffset + j, slotOffset + j);
      }

      slotOffset += textureHandles[Material::DIFFUSETEX].size();

      for(unsigned int j = 0; j < textureHandles[Material::NORMALTEX].size(); j++)
      {
        Texture *texture = m_textureManager->getObject(textureHandles[Material::NORMALTEX][j]);
        texture->setTexture(slotOffset + j, slotOffset + j);
      }

      slotOffset += textureHandles[Material::NORMALTEX].size();

      for(unsigned int j = 0; j < textureHandles[Material::SPECULARTEX].size(); j++)
      {
        Texture *texture = m_textureManager->getObject(textureHandles[Material::SPECULARTEX][j]);
        texture->setTexture(slotOffset + j, slotOffset + j);
      }

      slotOffset += textureHandles[Material::SPECULARTEX].size();

      for(unsigned int j = 0; j < textureHandles[Material::DISPLACEMENTTEX].size(); j++)
      {
        Texture *texture = m_textureManager->getObject(textureHandles[Material::DISPLACEMENTTEX][j]);
        texture->setTexture(slotOffset + j, slotOffset + j);
      }

      return true;
    }

    void RenderGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
      std::vector< std::vector<util::ResourceHandle> >& textureHandles = treeNode->getTextureHandles();

      for(unsigned int j = 0; j < textureHandles[Material::DIFFUSETEX].size(); j++)
      {
        Texture *texture = m_textureManager->getObject(textureHandles[Material::DIFFUSETEX][j]);
        texture->unsetTexture();
      }

      for(unsigned int j = 0; j < textureHandles[Material::NORMALTEX].size(); j++)
      {
        Texture *texture = m_textureManager->getObject(textureHandles[Material::NORMALTEX][j]);
        texture->unsetTexture();
      }

      for(unsigned int j = 0; j < textureHandles[Material::SPECULARTEX].size(); j++)
      {
        Texture *texture = m_textureManager->getObject(textureHandles[Material::SPECULARTEX][j]);
        texture->unsetTexture();
      }

      for(unsigned int j = 0; j < textureHandles[Material::DISPLACEMENTTEX].size(); j++)
      {
        Texture *texture = m_textureManager->getObject(textureHandles[Material::DISPLACEMENTTEX][j]);
        texture->unsetTexture();
      }
    }

    bool RenderGeometryTraverser::preTraverse(StaticRenderNode* treeNode)
    {
      treeNode->rasterizeGeometry();

      return true;
    }

    void RenderGeometryTraverser::postTraverse(StaticRenderNode* treeNode)
    {
    }

    bool RenderGeometryTraverser::preTraverse(AnimatedRenderNode* treeNode)
    {
      treeNode->rasterizeGeometry();

      return true;
    }

    void RenderGeometryTraverser::postTraverse(AnimatedRenderNode* treeNode)
    {
    }
	}
}