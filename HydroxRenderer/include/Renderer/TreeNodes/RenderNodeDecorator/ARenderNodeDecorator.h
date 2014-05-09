#ifndef ARENDERNODEDECORATOR_H_
#define ARENDERNODEDECORATOR_H_

#include <list>
#include <map>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
  namespace xBar
  {
    class SkinnedGeometryContainer;
    class StaticGeometryContainer;
  }

	namespace renderer
	{
    class Traverser;
    class DrawCommandInterface;
    class MatrixBufferInterface;

    class ARenderNodeDecorator : public IRenderNode
    {
    public:

      ARenderNodeDecorator(IRenderNode *renderNode);
      virtual ~ARenderNodeDecorator() = 0;

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

      unsigned int getMaxGeometry() const;
      unsigned int getMaxMaterials() const;
      unsigned int getMaxBones() const;

    protected:

      IRenderNode *m_renderNode;
    };
	}
}

#endif
