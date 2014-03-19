#ifndef RENDERNODE_H_
#define RENDERNODE_H_

#include <list>
#include <map>

#include <Utilities/Math/Math.hpp>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
	namespace renderer
	{
    class RenderNode : public IRenderNode
    {
    public:

      RenderNode(unsigned int maxGeometry);
      virtual ~RenderNode();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      unsigned int getInstanceCount();

      bool hasGeometryChanged();

      bool hasInstanceNumberChanged();

      bool isInstanced();

      bool isEmpty();

      void frustumCulling();

      void rasterizeGeometry();

      void updateBuffer();

    protected:

      bool m_geometryChanged;
      bool m_instanceNumberChanged;

      unsigned int m_maxGeometry;
      unsigned int m_instanceNumber;

      std::map<util::ResourceHandle, std::list<uint64_t>, Less> m_meshes;
    };
	}
}

#endif
