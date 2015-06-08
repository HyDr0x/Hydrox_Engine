#ifndef SHADERNODE_H_
#define SHADERNODE_H_

#include <vector>

#include <Utilities/Math/Math.hpp>

#include <Shader/ShaderContainer.h>

namespace he
{
  namespace renderer
  {
    class ShaderNode
    {
    public:

      ShaderNode();
      ~ShaderNode();

      void initialize(sh::RenderShaderHandle shaderHandle);

      bool isShader(sh::RenderShaderHandle shaderHandle) const;

      sh::RenderShaderHandle getShaderHandle() const;

    private:

      sh::RenderShaderHandle m_shaderHandle;
    };
  }
}

#endif
