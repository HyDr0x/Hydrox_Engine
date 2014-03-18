#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

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
    IRenderNode* RenderNodeFactory::createRenderNode(RenderNodeType nodeType, bool instanced, unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager)
    {
      switch(nodeType)
      {
      case SkinnedIndexedNode:
        return new MaterialDecorator(new SkinnedGeometryDecorator(new DrawElementsDecorator(new RenderNode(maxGeometry), instanced, GLINDEXTYPE, primitiveType, vertexStride, singletonManager), maxBones), maxMaterials, singletonManager);
        break;
      case NonSkinnedIndexedNode:
        return new MaterialDecorator(new StaticGeometryDecorator(new DrawElementsDecorator(new RenderNode(maxGeometry), instanced, GLINDEXTYPE, primitiveType, vertexStride, singletonManager)), maxMaterials, singletonManager);
        break;
      case SkinnedNonIndexedNode:
        return new MaterialDecorator(new SkinnedGeometryDecorator(new DrawElementsDecorator(new RenderNode(maxGeometry), instanced, GLINDEXTYPE, primitiveType, vertexStride, singletonManager), maxBones), maxMaterials, singletonManager);
        break;
      default:
      case NonSkinnedNonIndexedNode:
        return new MaterialDecorator(new StaticGeometryDecorator(new DrawArrayDecorator(new RenderNode(maxGeometry), instanced, primitiveType, vertexStride, singletonManager)), maxMaterials, singletonManager);
        break;
      };
    }
  }
}