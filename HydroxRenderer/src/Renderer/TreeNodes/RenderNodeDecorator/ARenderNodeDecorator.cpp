#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
  namespace renderer
  {
    ARenderNodeDecorator::ARenderNodeDecorator(IRenderGroup *renderNode) : m_renderNode(renderNode)
    {
    }

    ARenderNodeDecorator::~ARenderNodeDecorator()
    {
      delete m_renderNode;
    }

    bool ARenderNodeDecorator::containsContainer(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      return m_renderNode->containsContainer(geometryContainer);
    }

    bool ARenderNodeDecorator::insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      return m_renderNode->insertGeometry(geometryContainer);
    }

    bool ARenderNodeDecorator::removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
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

    void ARenderNodeDecorator::rasterizeShadowGeometry() const
    {
      m_renderNode->rasterizeShadowGeometry();
    }

    void ARenderNodeDecorator::rasterizeReflectiveShadowGeometry() const
    {
      m_renderNode->rasterizeReflectiveShadowGeometry();
    }

    void ARenderNodeDecorator::rasterizeIndexGeometry() const
    {
      m_renderNode->rasterizeIndexGeometry();
    }

    void ARenderNodeDecorator::rasterizeGeometry() const
    {
      m_renderNode->rasterizeGeometry();
    }

    void ARenderNodeDecorator::updateBuffer()
    {
      m_renderNode->updateBuffer();
    }

    unsigned int ARenderNodeDecorator::getCacheNumber() const
    {
      return m_renderNode->getCacheNumber();
    }
  }
}
