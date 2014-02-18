#include "Renderer/RenderTree/RemoveGeometryTraverser.h"

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
    RemoveGeometryTraverser::RemoveGeometryTraverser(util::SingletonManager *singletonManager)
    {
      m_node = nullptr;

      m_modelManager = singletonManager->getService<ModelManager>();
      m_materialManager = singletonManager->getService<MaterialManager>();
    }

    RemoveGeometryTraverser::~RemoveGeometryTraverser()
    {
    }

    void RemoveGeometryTraverser::setNode(sg::GeoNode *node)
    {
      m_node = node;

      m_vertexDeclaration = m_modelManager->getObject(m_node->getMeshHandle())->getVertexDeclarationFlags();

      Material *material = m_materialManager->getObject(m_node->getMaterialHandle());
      m_shaderHandle = material->getShaderHandle();

      m_textureHandles.resize(4);

      unsigned int texNum = material->getTextureNumber(Material::DIFFUSETEX);
      m_textureHandles[Material::DIFFUSETEX].resize(texNum);

      for(unsigned int i = 0; i < texNum; i++)
      {
        m_textureHandles[Material::DIFFUSETEX][i] = material->getTextureHandle(Material::DIFFUSETEX, i);
      }

      texNum = material->getTextureNumber(Material::NORMALTEX);
      m_textureHandles[Material::NORMALTEX].resize(texNum);

      for(unsigned int i = 0; i < texNum; i++)
      {
        m_textureHandles[Material::NORMALTEX][i] = material->getTextureHandle(Material::NORMALTEX, i);
      }

      texNum = material->getTextureNumber(Material::SPECULARTEX);
      m_textureHandles[Material::SPECULARTEX].resize(texNum);

      for(unsigned int i = 0; i < texNum; i++)
      {
        m_textureHandles[Material::SPECULARTEX][i] = material->getTextureHandle(Material::SPECULARTEX, i);
      }

      texNum = material->getTextureNumber(Material::DISPLACEMENTTEX);
      m_textureHandles[Material::DISPLACEMENTTEX].resize(texNum);

      for(unsigned int i = 0; i < texNum; i++)
      {
        m_textureHandles[Material::DISPLACEMENTTEX][i] = material->getTextureHandle(Material::DISPLACEMENTTEX, i);
      }
    }

    bool RemoveGeometryTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return treeNode->isMesh(m_vertexDeclaration);
    }

    void RemoveGeometryTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
    }

    bool RemoveGeometryTraverser::preTraverse(ShaderNode* treeNode)
    {
      return treeNode->isShader(m_shaderHandle);
    }

    void RemoveGeometryTraverser::postTraverse(ShaderNode* treeNode)
    {
    }

    bool RemoveGeometryTraverser::preTraverse(TextureNode* treeNode)
    {
      return treeNode->isTexture(m_textureHandles);
    }

    void RemoveGeometryTraverser::postTraverse(TextureNode* treeNode)
    {
    }

    bool RemoveGeometryTraverser::preTraverse(StaticRenderNode* treeNode)
    {
      treeNode->removeGeometry(m_node);

      return true;
    }

    void RemoveGeometryTraverser::postTraverse(StaticRenderNode* treeNode)
    {
    }

    bool RemoveGeometryTraverser::preTraverse(AnimatedRenderNode* treeNode)
    {
      treeNode->removeGeometry(m_node);

      return true;
    }

    void RemoveGeometryTraverser::postTraverse(AnimatedRenderNode* treeNode)
    {
    }
	}
}