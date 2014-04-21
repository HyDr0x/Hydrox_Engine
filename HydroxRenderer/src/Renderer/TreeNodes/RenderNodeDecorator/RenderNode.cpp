#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/Traverser.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
	namespace renderer
	{
    RenderNode::RenderNode(unsigned int maxGeometry) : 
      m_instanceNumber(0), 
      m_maxGeometry(maxGeometry),
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

    bool RenderNode::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_instanceNumber >= m_maxGeometry)
      {
        return false;
      }

      m_instanceNumberChanged = true;

      m_instances.push_back(geometryContainer.getHash());

      m_instanceNumber++;

      return true;
    }

    unsigned int RenderNode::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      bool deleted = false;
      unsigned int instanceIndex = 0;
      for(std::list<uint64_t>::const_iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++, instanceIndex++)
      {
        if(*instanceIterator == geometryContainer.getHash())
        {
          m_instances.erase(instanceIterator);
          deleted = true;
          break;
        }
      }

      if(!deleted)//the node wasnt in the list --> wrong render node
      {
        return ~0;
      }

      m_instanceNumber--;

      m_instanceNumberChanged = true;

      return instanceIndex;
    }

    unsigned int RenderNode::getInstanceNumber()
    {
      return m_instanceNumber;
    }

    bool RenderNode::hasInstanceNumberChanged()
    {
      return m_instanceNumberChanged;
    }

    void RenderNode::frustumCulling()
    {
      ComputeShader::setUniform(0, GL_UNSIGNED_INT, &m_instanceNumber);

      ComputeShader::dispatchComputeShader(256, 1, 1);
    }

    void RenderNode::rasterizeGeometry()
    {
    }

    void RenderNode::updateBuffer()
    {
      m_instanceNumberChanged = false;
    }
	}
}
