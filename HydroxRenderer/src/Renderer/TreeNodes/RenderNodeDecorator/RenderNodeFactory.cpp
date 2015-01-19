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
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    util::SharedPointer<IRenderGroup> RenderNodeFactory::createRenderNode(util::Flags<xBar::RenderNodeType> nodeType, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager)
    {
      util::SharedPointer<RenderOptions> options = singletonManager->getService<RenderOptions>();

      switch(nodeType.toInt())
      {
      case xBar::SKINNEDNODE | xBar::INDEXEDNODE:
        return util::SharedPointer<IRenderGroup>(new MaterialDecorator(new SkinnedGeometryDecorator(new DrawElementsDecorator(new SkinnedRenderNode(options), GLINDEXTYPE, primitiveType, vertexStride, singletonManager), options), singletonManager));
        break;
      case xBar::INDEXEDNODE:
        return util::SharedPointer<IRenderGroup>(new MaterialDecorator(new StaticGeometryDecorator(new DrawElementsDecorator(new StaticRenderNode(options), GLINDEXTYPE, primitiveType, vertexStride, singletonManager), options), singletonManager));
        break;
      case xBar::SKINNEDNODE:
        return util::SharedPointer<IRenderGroup>(new MaterialDecorator(new SkinnedGeometryDecorator(new DrawArrayDecorator(new SkinnedRenderNode(options), primitiveType, vertexStride, singletonManager), options), singletonManager));
        break;
      default:
        return util::SharedPointer<IRenderGroup>(new MaterialDecorator(new StaticGeometryDecorator(new DrawArrayDecorator(new StaticRenderNode(options), primitiveType, vertexStride, singletonManager), options), singletonManager));
        break;
      };
    }
  }
}