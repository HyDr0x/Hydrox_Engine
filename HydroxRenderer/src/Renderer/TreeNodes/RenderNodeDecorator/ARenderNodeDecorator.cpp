#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/InsertGeometryTraverser.h"

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
      return m_renderNode->preTraverse(traverser);
    }

    void ARenderNodeDecorator::postTraverse(Traverser* traverser)
    {
      m_renderNode->postTraverse(traverser);
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
