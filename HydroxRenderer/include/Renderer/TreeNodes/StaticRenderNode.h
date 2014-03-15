#ifndef STATICRENDERNODE_H_
#define STATICRENDERNODE_H_

#include <list>
#include <map>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/Pipeline/FrustumCullingGPU.h"

#include "Renderer/TreeNodes/RenderNode.h"

namespace he
{
  namespace xBar
  {
    class StaticGeometryContainer;
  }

	namespace renderer
	{
    class Traverser;
    class DrawCommandInterface;

    class StaticRenderNode : public RenderNode
    {
    public:

      StaticRenderNode(unsigned int maxMaterials, unsigned int maxGeometry);
      virtual ~StaticRenderNode();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      void initialize(bool indexed, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);

      bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      bool isEmpty();

      void frustumCulling();
      void rasterizeGeometry();

    protected:

      std::map<util::ResourceHandle, std::list<xBar::StaticGeometryContainer>, Less> m_meshHandles;
    };
	}
}

#endif
