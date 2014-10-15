#ifndef RENDERNODEFACTORY_H_
#define RENDERNODEFACTORY_H_

#include <GL/glew.h>

#include <XBar/IGeometryContainer.h>

#include <Utilities/Pointer/SharedPointer.h>
#include <Utilities/Miscellaneous/Flags.hpp>

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class IRenderGroup;
    class RenderOptions;

    class RenderNodeFactory
    {
    public:

      static util::SharedPointer<IRenderGroup> createRenderNode(util::Flags<xBar::RenderNodeType> nodeType, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);

    private:

    };
  }
}

#endif
