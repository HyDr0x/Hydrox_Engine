#include "Renderer/TreeNodes/RenderNodeDecorator/SkinnedRenderNode.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    SkinnedRenderNode::SkinnedRenderNode(const RenderOptions& options) :  
      m_options(options),
      m_instanceNumberChanged(false)
    {
    }

    SkinnedRenderNode::~SkinnedRenderNode()
    {
    }

    bool SkinnedRenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void SkinnedRenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool SkinnedRenderNode::insertGeometry(const xBar::IGeometryContainer& geometryContainer)
    {
      if(m_instances.size() >= m_options.maxGeometry)
      {
        return false;
      }

      m_instanceNumberChanged = true;

      m_instances.push_back(dynamic_cast<const xBar::SkinnedGeometryContainer&>(geometryContainer));

      return true;
    }

    bool SkinnedRenderNode::removeGeometry(const xBar::IGeometryContainer& geometryContainer)
    {
      for(std::list<const xBar::SkinnedGeometryContainer>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
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
      ComputeShader::setUniform(0, GL_UNSIGNED_INT, &instanceNumber);

      ComputeShader::dispatchComputeShader(256, 1, 1);
    }

    void SkinnedRenderNode::rasterizeGeometry() const
    {
    }

    void SkinnedRenderNode::updateBuffer()
    {
      m_instanceNumberChanged = false;
    }

    unsigned int SkinnedRenderNode::getMaxGeometry() const
    {
      return m_options.maxGeometry;
    }

    unsigned int SkinnedRenderNode::getMaxMaterials() const
    {
      return m_options.maxMaterials;
    }

    unsigned int SkinnedRenderNode::getMaxBones() const
    {
      return m_options.maxBones;
    }
  }
}
