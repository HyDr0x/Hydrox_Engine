#ifndef ARENDERNODEDECORATOR_H_
#define ARENDERNODEDECORATOR_H_

#include <list>
#include <map>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <DataBase/ResourceManager.hpp>

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
    class DrawCommandInterface;
    class MatrixBufferInterface;

    class ARenderNodeDecorator : public IRenderNode
    {
    public:

      ARenderNodeDecorator(IRenderNode *renderNode);
      virtual ~ARenderNodeDecorator() = 0;

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool preTraverse(ConstTraverser* traverser) const override;
      virtual void postTraverse(ConstTraverser* traverser) const override;

      virtual bool insertGeometry(const xBar::IGeometryContainer& geometryContainer);
      virtual bool removeGeometry(const xBar::IGeometryContainer& geometryContainer);

      virtual bool isEndInstanceIterator() const;
      virtual void resetInstanceIterator();
      virtual const xBar::IGeometryContainer& incInstanceIterator();

      virtual unsigned int getInstanceNumber() const;

      virtual bool hasInstanceNumberChanged() const;

      virtual void frustumCulling(unsigned int viewProjectionMatrixID) const;

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
