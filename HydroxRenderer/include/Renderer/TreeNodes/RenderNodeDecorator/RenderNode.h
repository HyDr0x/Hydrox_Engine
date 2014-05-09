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
    struct RenderOptions;

    class RenderNode : public IRenderNode
    {
    public:

      RenderNode(const RenderOptions& options);
      virtual ~RenderNode();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool insertGeometry(xBar::SkinnedGeometryContainer& geometryContainer);

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual std::list<xBar::StaticGeometryContainer*>& getInstances();

      virtual unsigned int getInstanceNumber();

      virtual bool hasInstanceNumberChanged();

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual void updateBuffer();

      virtual unsigned int getMaxGeometry();
      virtual unsigned int getMaxMaterials();
      virtual unsigned int getMaxBones();

    protected:

      bool m_instanceNumberChanged;

      const RenderOptions& m_options;

      std::list<xBar::StaticGeometryContainer*> m_instances;
    };
	}
}

#endif
