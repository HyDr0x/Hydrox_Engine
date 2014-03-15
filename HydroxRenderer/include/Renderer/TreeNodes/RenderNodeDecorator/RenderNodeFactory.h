#ifndef RENDERNODEFACTORY_H_
#define RENDERNODEFACTORY_H_

#include <GL/glew.h>

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

	namespace renderer
	{
    class IRenderNode;

    enum RenderNodeType
    {
      SkinnedIndexedNode,
      NonSkinnedIndexedNode,
      SkinnedNonIndexedNode,
      NonSkinnedNonIndexedNode,
    };

    class RenderNodeFactory
    {
    public:

      static IRenderNode* createRenderNode(RenderNodeType nodeType, unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);

    private:

    };
  }
}

#endif
