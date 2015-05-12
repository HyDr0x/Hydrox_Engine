#ifndef SHADERNODE_H_
#define SHADERNODE_H_

#include <vector>

#include <Utilities/Math/Math.hpp>

#include <Shader/ShaderContainer.h>

#include "Renderer/TreeNodes/GroupNode.h"

namespace he
{
  namespace renderer
  {
    class ShaderNode : public GroupNode
    {
    public:

      ShaderNode();
      ~ShaderNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      bool preTraverse(ConstTraverser* traverser) const;
      void postTraverse(ConstTraverser* traverser) const;

      void initialize(sh::RenderShaderHandle shaderHandle);

      bool isShader(sh::RenderShaderHandle shaderHandle) const;

      sh::RenderShaderHandle getShaderHandle() const;

    private:

      sh::RenderShaderHandle m_shaderHandle;
    };
  }
}

#endif
