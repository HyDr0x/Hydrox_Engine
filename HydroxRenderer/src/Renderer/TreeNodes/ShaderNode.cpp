#include "Renderer/TreeNodes/ShaderNode.h"

#include "Renderer/Traverser/Traverser.h"

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

    void ShaderNode::initialize(util::ResourceHandle shaderHandle)
    {
      m_shaderHandle = shaderHandle;
    }

    bool ShaderNode::isShader(util::ResourceHandle shaderHandle) const
    {
      return m_shaderHandle == shaderHandle;
    }

    util::ResourceHandle ShaderNode::getShaderHandle() const
    {
      return m_shaderHandle;
    }
  }
}
