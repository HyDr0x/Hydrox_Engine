#ifndef SHADERNODE_H_
#define SHADERNODE_H_

#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

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

      void initialize(util::ResourceHandle shaderHandle);

      bool isShader(util::ResourceHandle shaderHandle) const;

      util::ResourceHandle getShaderHandle() const;

    private:

      util::ResourceHandle m_shaderHandle;
    };
  }
}

#endif
