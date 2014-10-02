#ifndef RENDERNODEFACTORY_H_
#define RENDERNODEFACTORY_H_

#include <GL/glew.h>

#include <Utilities/Miscellaneous/Flags.hpp>

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class IRenderNode;
    struct RenderOptions;

    enum RenderNodeType
    {
      SKINNEDNODE = 1,
      INDEXEDNODE = 2,
    };

    class RenderNodeFactory
    {
    public:

      static IRenderNode* createRenderNode(util::Flags<RenderNodeType> nodeType, const RenderOptions& options, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);

      static IRenderNode* createShadowRenderNode(util::Flags<RenderNodeType> nodeType, const RenderOptions& options, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);

    private:

    };
  }
}

#endif
