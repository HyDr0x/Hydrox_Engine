#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Traverser/ConstTraverser.h"
#include "Renderer/Traverser/InsertShadowGeometryTraverser.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
  namespace renderer
  {
    ARenderNodeDecorator::ARenderNodeDecorator(IRenderNode *renderNode) : m_renderNode(renderNode)
    {
    }

    ARenderNodeDecorator::~ARenderNodeDecorator()
    {
      delete m_renderNode;
    }

    bool ARenderNodeDecorator::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void ARenderNodeDecorator::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool ARenderNodeDecorator::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void ARenderNodeDecorator::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    bool ARenderNodeDecorator::insertGeometry(const xBar::IGeometryContainer& geometryContainer)
    {
      return m_renderNode->insertGeometry(geometryContainer);
    }

    bool ARenderNodeDecorator::removeGeometry(const xBar::IGeometryContainer& geometryContainer)
    {
      return m_renderNode->removeGeometry(geometryContainer);
    }

    bool ARenderNodeDecorator::isEndInstanceIterator() const
    {
      return m_renderNode->isEndInstanceIterator();
    }

    void ARenderNodeDecorator::resetInstanceIterator()
    {
      m_renderNode->resetInstanceIterator();
    }

    const xBar::IGeometryContainer& ARenderNodeDecorator::incInstanceIterator()
    {
      return m_renderNode->incInstanceIterator();
    }

    unsigned int ARenderNodeDecorator::getInstanceNumber() const
    {
      return m_renderNode->getInstanceNumber();
    }

    bool ARenderNodeDecorator::hasInstanceNumberChanged() const
    {
      return m_renderNode->hasInstanceNumberChanged();
    }

    void ARenderNodeDecorator::frustumCulling() const
    {
      m_renderNode->frustumCulling();
    }

    void ARenderNodeDecorator::rasterizeGeometry() const
    {
      m_renderNode->rasterizeGeometry();
    }

    void ARenderNodeDecorator::updateBuffer()
    {
      m_renderNode->updateBuffer();
    }

    unsigned int ARenderNodeDecorator::getMaxGeometry() const
    {
      return m_renderNode->getMaxGeometry();
    }

    unsigned int ARenderNodeDecorator::getMaxMaterials() const
    {
      return m_renderNode->getMaxMaterials();
    }

    unsigned int ARenderNodeDecorator::getMaxBones() const
    {
      return m_renderNode->getMaxBones();
    }
  }
}
