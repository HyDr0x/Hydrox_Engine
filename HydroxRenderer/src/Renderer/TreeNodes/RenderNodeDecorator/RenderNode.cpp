#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNode.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Pipeline/RenderingOptions.h"

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

    bool RenderNode::insertGeometry(xBar::SkinnedGeometryContainer& geometryContainer)
    {
      if(m_instances.size() >= m_options.m_maxGeometry)
      {
        return false;
      }

      m_instanceNumberChanged = true;

      m_instances.push_back(new xBar::SkinnedGeometryContainer(geometryContainer));

      return true;
    }

    bool RenderNode::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_instances.size() >= m_options.m_maxGeometry)
      {
        return false;
      }

      m_instanceNumberChanged = true;

      m_instances.push_back(new xBar::StaticGeometryContainer(geometryContainer));

      return true;
    }

    bool RenderNode::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      bool deleted = false;

      for(std::list<xBar::StaticGeometryContainer*>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
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

    std::list<xBar::StaticGeometryContainer*>& RenderNode::getInstances()
    {
      return m_instances;
    }

    unsigned int RenderNode::getInstanceNumber()
    {
      return m_instances.size();
    }

    bool RenderNode::hasInstanceNumberChanged()
    {
      return m_instanceNumberChanged;
    }

    void RenderNode::frustumCulling()
    {
      unsigned int instanceNumber = m_instances.size();
      ComputeShader::setUniform(0, GL_UNSIGNED_INT, &instanceNumber);

      ComputeShader::dispatchComputeShader(256, 1, 1);
    }

    void RenderNode::rasterizeGeometry()
    {
    }

    void RenderNode::updateBuffer()
    {
      m_instanceNumberChanged = false;
    }

    unsigned int RenderNode::getMaxGeometry()
    {
      return m_options.m_maxGeometry;
    }

    unsigned int RenderNode::getMaxMaterials()
    {
      return m_options.m_maxMaterials;
    }

    unsigned int RenderNode::getMaxBones()
    {
      return m_options.m_maxBones;
    }
	}
}
