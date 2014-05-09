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

      virtual bool insertGeometry(const xBar::SkinnedGeometryContainer& geometryContainer);

      virtual bool insertGeometry(const xBar::StaticGeometryContainer& geometryContainer);
      virtual bool removeGeometry(const xBar::StaticGeometryContainer& geometryContainer);

      virtual const std::list<const xBar::StaticGeometryContainer*>& getInstances() const;

      virtual unsigned int getInstanceNumber() const;

      virtual bool hasInstanceNumberChanged() const;

      virtual void frustumCulling() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

      virtual unsigned int getMaxGeometry() const;
      virtual unsigned int getMaxMaterials() const;
      virtual unsigned int getMaxBones() const;

    protected:

      bool m_instanceNumberChanged;

      const RenderOptions& m_options;

      std::list<const xBar::StaticGeometryContainer*> m_instances;
    };
	}
}

#endif
