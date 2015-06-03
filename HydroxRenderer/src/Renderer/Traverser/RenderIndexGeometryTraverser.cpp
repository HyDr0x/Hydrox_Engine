#include "Renderer/Traverser/RenderIndexGeometryTraverser.h"

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
    RenderIndexGeometryTraverser::RenderIndexGeometryTraverser()
    {
    }

    RenderIndexGeometryTraverser::~RenderIndexGeometryTraverser()
    {
    }

    void RenderIndexGeometryTraverser::initialize(util::SingletonManager *singletonManager)
    {
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();
    }

    void RenderIndexGeometryTraverser::setGlobalBufferResolution(unsigned int globalBufferResolution)
    {
      m_globalBufferResolution = globalBufferResolution;
    }

    bool RenderIndexGeometryTraverser::preTraverse(GroupNode * treeNode)
    {
      m_globalCacheOffset = 0;//reset global cache offset counter

      return true;
    }

    bool RenderIndexGeometryTraverser::preTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->setVertexArray();

      return true;
    }

    void RenderIndexGeometryTraverser::postTraverse(VertexDeclarationNode * treeNode)
    {
      treeNode->unsetVertexArray();
    }

    bool RenderIndexGeometryTraverser::preTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useShader();

      sh::RenderShader::setUniform(1, GL_UNSIGNED_INT, &m_globalBufferResolution);

      return true;
    }

    void RenderIndexGeometryTraverser::postTraverse(ShaderNode * treeNode)
    {
      const sh::RenderShader& shader = m_shaderContainer->getRenderShader(treeNode->getShaderHandle());

      shader.useNoShader();
    }

    bool RenderIndexGeometryTraverser::preTraverse(RenderNode * treeNode)
    {
      sh::RenderShader::setUniform(0, GL_UNSIGNED_INT, &m_globalCacheOffset);

      treeNode->getRenderGroup()->rasterizeIndexGeometry();

      m_globalCacheOffset += treeNode->getRenderGroup()->getCacheNumber();//need to be increased with cache number per node

      return true;
    }

    void RenderIndexGeometryTraverser::postTraverse(RenderNode * treeNode)
    {
    }
  }
}