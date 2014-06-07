#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/DrawArrayDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/DrawElementsDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/SkinnedGeometryDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/StaticGeometryDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/MaterialDecorator.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/StaticRenderNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/SkinnedRenderNode.h"
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
        return new MaterialDecorator(new SkinnedGeometryDecorator(new DrawElementsDecorator(new SkinnedRenderNode(options), GLINDEXTYPE, primitiveType, vertexStride, singletonManager)), singletonManager);
        break;
      case INDEXEDNODE:
        return new MaterialDecorator(new StaticGeometryDecorator(new DrawElementsDecorator(new StaticRenderNode(options), GLINDEXTYPE, primitiveType, vertexStride, singletonManager)), singletonManager);
        break;
      case SKINNEDNODE:
        return new MaterialDecorator(new SkinnedGeometryDecorator(new DrawArrayDecorator(new SkinnedRenderNode(options), primitiveType, vertexStride, singletonManager)), singletonManager);
        break;
      default:
        return new MaterialDecorator(new StaticGeometryDecorator(new DrawArrayDecorator(new StaticRenderNode(options), primitiveType, vertexStride, singletonManager)), singletonManager);
        break;
      };
    }
  }
}