#include "Renderer/Renderpass/ShaderNode.h"

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
