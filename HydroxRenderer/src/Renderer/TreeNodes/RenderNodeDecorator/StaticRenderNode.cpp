#include "Renderer/TreeNodes/RenderNodeDecorator/StaticRenderNode.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Traverser/ConstTraverser.h"

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    StaticRenderNode::StaticRenderNode(const RenderOptions& options) :  
      m_options(options),
      m_instanceNumberChanged(false)
    {
    }

    StaticRenderNode::~StaticRenderNode()
    {
    }

    bool StaticRenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void StaticRenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool StaticRenderNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void StaticRenderNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    bool StaticRenderNode::insertGeometry(const xBar::IGeometryContainer& geometryContainer)
    {
      if(m_instances.size() >= m_options.maxGeometry)
      {
        return false;
      }

      m_instanceNumberChanged = true;

      m_instances.push_back(dynamic_cast<const xBar::StaticGeometryContainer&>(geometryContainer));

      return true;
    }

    bool StaticRenderNode::removeGeometry(const xBar::IGeometryContainer& geometryContainer)
    {
      for(std::list<const xBar::StaticGeometryContainer>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
      {
        if((*instanceIterator) == geometryContainer)
        {
          m_instances.erase(instanceIterator);
          m_instanceNumberChanged = true;
          return true;
        }
      }    

      return false;
    }

    unsigned int StaticRenderNode::getInstanceNumber() const
    {
      return m_instances.size();
    }

    bool StaticRenderNode::hasInstanceNumberChanged() const
    {
      return m_instanceNumberChanged;
    }

    void StaticRenderNode::frustumCulling() const
    {
      unsigned int instanceNumber = m_instances.size();
      db::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &instanceNumber);
      db::ComputeShader::dispatchComputeShader(256, 1, 1);
    }

    void StaticRenderNode::rasterizeGeometry() const
    {
    }

    void StaticRenderNode::updateBuffer()
    {
      m_instanceNumberChanged = false;
    }

    unsigned int StaticRenderNode::getMaxGeometry() const
    {
      return m_options.maxGeometry;
    }

    unsigned int StaticRenderNode::getMaxMaterials() const
    {
      return m_options.maxMaterials;
    }

    unsigned int StaticRenderNode::getMaxBones() const
    {
      return m_options.maxBones;
    }
  }
}
