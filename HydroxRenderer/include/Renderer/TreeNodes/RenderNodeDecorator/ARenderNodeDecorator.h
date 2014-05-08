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

      virtual bool insertGeometry(xBar::SkinnedGeometryContainer& geometryContainer);

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual std::list<xBar::StaticGeometryContainer*>& getInstances();

      virtual unsigned int getInstanceNumber();

      virtual bool hasInstanceNumberChanged();

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual void updateBuffer();

      unsigned int getMaxGeometry();
      unsigned int getMaxMaterials();
      unsigned int getMaxBones();

    protected:

      IRenderNode *m_renderNode;
    };
	}
}

#endif
