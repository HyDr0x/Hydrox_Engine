#include "Renderer/TreeNodes/ShaderNode.h"

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Traverser/ConstTraverser.h"

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

    bool ShaderNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void ShaderNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    void ShaderNode::initialize(sh::RenderShaderHandle shaderHandle)
    {
      m_shaderHandle = shaderHandle;
    }

    bool ShaderNode::isShader(sh::RenderShaderHandle shaderHandle) const
    {
      return m_shaderHandle == shaderHandle;
    }

    sh::RenderShaderHandle ShaderNode::getShaderHandle() const
    {
      return m_shaderHandle;
    }
  }
}
