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

    bool ARenderNodeDecorator::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
     return m_renderNode->insertGeometry(geometryContainer);
    }

    bool ARenderNodeDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      return m_renderNode->removeGeometry(geometryContainer);
    }

    unsigned int ARenderNodeDecorator::getInstanceCount()
    {
      return m_renderNode->getInstanceCount();
    }

    bool ARenderNodeDecorator::hasGeometryChanged()
    {
      return m_renderNode->hasGeometryChanged();
    }

    bool ARenderNodeDecorator::hasInstanceNumberChanged()
    {
      return m_renderNode->hasInstanceNumberChanged();
    }

    bool ARenderNodeDecorator::isInstanced()
    {
      return m_renderNode->isInstanced();
    }

    bool ARenderNodeDecorator::isEmpty()
    {
      return m_renderNode->isEmpty();
    }

    void ARenderNodeDecorator::frustumCulling()
    {
      m_renderNode->frustumCulling();
    }

    void ARenderNodeDecorator::rasterizeGeometry()
    {
      m_renderNode->rasterizeGeometry();
    }

    void ARenderNodeDecorator::updateBuffer()
    {
      m_renderNode->updateBuffer();
    }
	}
}
