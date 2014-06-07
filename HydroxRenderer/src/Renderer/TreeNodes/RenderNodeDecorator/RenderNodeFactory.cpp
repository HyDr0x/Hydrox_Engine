#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/DrawArrayDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/DrawElementsDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/SkinnedGeometryDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/StaticGeometryDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/MaterialDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
  namespace renderer
  {
    IRenderNode* RenderNodeFactory::createRenderNode(util::Flags<RenderNodeType> nodeType, const RenderOptions& options, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager)
    {
      switch(nodeType.toInt())
      {
      case SKINNEDNODE | INDEXEDNODE:
        return new MaterialDecorator(new SkinnedGeometryDecorator(new DrawElementsDecorator(new RenderNode(options), GLINDEXTYPE, primitiveType, vertexStride, singletonManager)), singletonManager);
        break;
      case INDEXEDNODE:
        return new MaterialDecorator(new StaticGeometryDecorator(new DrawElementsDecorator(new RenderNode(options), GLINDEXTYPE, primitiveType, vertexStride, singletonManager)), singletonManager);
        break;
      case SKINNEDNODE:
        return new MaterialDecorator(new SkinnedGeometryDecorator(new DrawElementsDecorator(new RenderNode(options), GLINDEXTYPE, primitiveType, vertexStride, singletonManager)), singletonManager);
        break;
      default:
        return new MaterialDecorator(new StaticGeometryDecorator(new DrawArrayDecorator(new RenderNode(options), primitiveType, vertexStride, singletonManager)), singletonManager);
        break;
      };
    }
  }
}