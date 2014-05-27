#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNode.h"

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
    RenderNode::RenderNode(const RenderOptions& options) :  
      m_options(options),
      m_instanceNumberChanged(false)
    {
    }

    RenderNode::~RenderNode()
    {
    }

    bool RenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void RenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool RenderNode::insertGeometry(const xBar::SkinnedGeometryContainer& geometryContainer)
    {
      if(m_instances.size() >= m_options.maxGeometry)
      {
        return false;
      }

      m_instanceNumberChanged = true;

      m_instances.push_back(new xBar::SkinnedGeometryContainer(geometryContainer));

      return true;
    }

    bool RenderNode::insertGeometry(const xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_instances.size() >= m_options.maxGeometry)
      {
        return false;
      }

      m_instanceNumberChanged = true;

      m_instances.push_back(new xBar::StaticGeometryContainer(geometryContainer));

      return true;
    }

    bool RenderNode::removeGeometry(const xBar::StaticGeometryContainer& geometryContainer)
    {
      bool deleted = false;

      for(std::list<const xBar::StaticGeometryContainer*>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
      {
        if((**instanceIterator) == geometryContainer)
        {
          delete *instanceIterator;
          m_instances.erase(instanceIterator);
          m_instanceNumberChanged = true;
          deleted = true;
          break;
        }
      }    

      return deleted;
    }

    const std::list<const xBar::StaticGeometryContainer*>& RenderNode::getInstances() const
    {
      return m_instances;
    }

    unsigned int RenderNode::getInstanceNumber() const
    {
      return m_instances.size();
    }

    bool RenderNode::hasInstanceNumberChanged() const
    {
      return m_instanceNumberChanged;
    }

    void RenderNode::frustumCulling() const
    {
      unsigned int instanceNumber = m_instances.size();
      ComputeShader::setUniform(0, GL_UNSIGNED_INT, &instanceNumber);

      ComputeShader::dispatchComputeShader(256, 1, 1);
    }

    void RenderNode::rasterizeGeometry() const
    {
    }

    void RenderNode::updateBuffer()
    {
      m_instanceNumberChanged = false;
    }

    unsigned int RenderNode::getMaxGeometry() const
    {
      return m_options.maxGeometry;
    }

    unsigned int RenderNode::getMaxMaterials() const
    {
      return m_options.maxMaterials;
    }

    unsigned int RenderNode::getMaxBones() const
    {
      return m_options.maxBones;
    }
	}
}
