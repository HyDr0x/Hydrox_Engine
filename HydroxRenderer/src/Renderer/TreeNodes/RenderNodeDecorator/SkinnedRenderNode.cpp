#include "Renderer/TreeNodes/RenderNodeDecorator/SkinnedRenderNode.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Traverser/ConstTraverser.h"

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    SkinnedRenderNode::SkinnedRenderNode(util::SharedPointer<RenderOptions> options) :
      m_options(options),
      m_instanceNumberChanged(false)
    {
    }

    SkinnedRenderNode::~SkinnedRenderNode()
    {
    }

    bool SkinnedRenderNode::containsContainer(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      for(std::list<const xBar::SkinnedGeometryContainer>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
      {
        if((*instanceIterator) == *geometryContainer)
        {
          return true;
        }
      }

      return false;
    }

    bool SkinnedRenderNode::insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      if(m_instances.size() >= m_options->maxGeometry)
      {
        return false;
      }

      m_instanceNumberChanged = true;

      m_instances.push_back(*geometryContainer.dynamic_pointer_cast<const xBar::SkinnedGeometryContainer>());

      return true;
    }

    bool SkinnedRenderNode::removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      for(std::list<const xBar::SkinnedGeometryContainer>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
      {
        if((*instanceIterator) == *geometryContainer)
        {
          m_instances.erase(instanceIterator);
          m_instanceNumberChanged = true;
          return true;
        }
      }    

      return false;
    }

    unsigned int SkinnedRenderNode::getInstanceNumber() const
    {
      return m_instances.size();
    }

    bool SkinnedRenderNode::hasInstanceNumberChanged() const
    {
      return m_instanceNumberChanged;
    }

    void SkinnedRenderNode::frustumCulling() const
    {
      unsigned int instanceNumber = m_instances.size();
      db::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &instanceNumber);
      db::ComputeShader::dispatchComputeShader(256, 1, 1);
    }

    void SkinnedRenderNode::rasterizeShadowGeometry() const
    {
    }

    void SkinnedRenderNode::rasterizeReflectiveShadowGeometry() const
    {
    }

    void SkinnedRenderNode::rasterizeIndexGeometry() const
    {
    }

    void SkinnedRenderNode::rasterizeIndirectLightingGeometry() const
    {
    }

    void SkinnedRenderNode::rasterizeGeometry() const
    {
    }

    void SkinnedRenderNode::updateBuffer()
    {
      m_instanceNumberChanged = false;
    }

    unsigned int SkinnedRenderNode::getCacheNumber() const
    {
      return 0;
    }
  }
}
