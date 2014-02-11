#ifndef SHADERNODE_H_
#define SHADERNODE_H_

#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/RenderTree/GroupNode.h"

namespace he
{
	namespace renderer
	{
    class Traverser;

    class ShaderNode : public GroupNode
    {
    public:

      ShaderNode();
      ~ShaderNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      TreeNode* createNewNode(InsertGeometryTraverser* traverser);

      void initialize(util::ResourceHandle shaderHandle);

      bool isShader(util::ResourceHandle shaderHandle);

      util::ResourceHandle getShaderHandle();

    private:

      util::ResourceHandle m_shaderHandle;
    };
	}
}

#endif
