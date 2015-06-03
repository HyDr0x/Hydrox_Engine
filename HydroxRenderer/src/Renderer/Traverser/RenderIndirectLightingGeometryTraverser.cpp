#include "Renderer/Traverser/RenderIndirectLightingGeometryTraverser.h"

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
    RenderIndirectLightingGeometryTraverser::RenderIndirectLightingGeometryTraverser()
    {
    }

    RenderIndirectLightingGeometryTraverser::~RenderIndirectLightingGeometryTraverser()
    {
    }

    void RenderIndirectLightingGeometryTraverser::initialize(util::SingletonManager *singletonManager, util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap)
    {
      m_normalMap = normalMap;
      m_materialMap = materialMap;

      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();
    }

    void RenderIndirectLightingGeometryTraverser::setGlobalBufferResolution(unsigned int globalBufferResolution)
    {
      m_globalBufferResolution = globalBufferResolution;
    }

    bool RenderIndirectLightingGeometryTraverser::preTraverse(GroupNode * treeNode)
    {
      m_globalCacheOffset = 0;//reset global cache offset counter

      return true;
    }

    bool RenderIndirectLightingGeometryTraverser::preTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->setVertexArray();

      return true;
    }

    void RenderIndirectLightingGeometryTraverser::postTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->unsetVertexArray();
    }

    bool RenderIndirectLightingGeometryTraverser::preTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useShader();

      sh::RenderShader::setUniform(2, GL_UNSIGNED_INT, &m_globalBufferResolution);

      m_normalMap->setTexture(0, 0);
      m_materialMap->setTexture(1, 1);

      return true;
    }

    void RenderIndirectLightingGeometryTraverser::postTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      m_materialMap->unsetTexture(1);
      m_normalMap->unsetTexture(0);

      shader.useNoShader();
    }

    bool RenderIndirectLightingGeometryTraverser::preTraverse(RenderNode * treeNode)
    {
      sh::RenderShader::setUniform(3, GL_UNSIGNED_INT, &m_globalCacheOffset);

      treeNode->getRenderGroup()->rasterizeIndirectLightingGeometry();

      m_globalCacheOffset += treeNode->getRenderGroup()->getCacheNumber();//need to be increased with cache number per node

      return true;
    }

    void RenderIndirectLightingGeometryTraverser::postTraverse(RenderNode * treeNode)
    {
    }
  }
}