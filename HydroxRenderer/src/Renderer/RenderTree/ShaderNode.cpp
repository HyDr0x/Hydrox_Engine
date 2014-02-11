#include "Renderer/RenderTree/ShaderNode.h"

#include "Renderer/RenderTree/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    ShaderNode::ShaderNode()
    {
    }

    ShaderNode::~ShaderNode()
    {
    }

    bool ShaderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void ShaderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    TreeNode* ShaderNode::createNewNode(InsertGeometryTraverser* traverser)
    {
      return traverser->createNewNode(this);
    }

    void ShaderNode::initialize(util::ResourceHandle shaderHandle)
    {
      m_shaderHandle = shaderHandle;
    }

    bool ShaderNode::isShader(util::ResourceHandle shaderHandle)
    {
      return m_shaderHandle == shaderHandle;
    }

    util::ResourceHandle ShaderNode::getShaderHandle()
    {
      return m_shaderHandle;
    }
  }
}
