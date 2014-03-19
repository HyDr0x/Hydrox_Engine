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
      m_geometryChanged(false),
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

      if(m_meshes[geometryContainer.getMeshHandle()].size() != 0)
      {
        m_instanceNumberChanged = true;
      }
      else
      {
        m_geometryChanged = true;
      }

      m_meshes[geometryContainer.getMeshHandle()].push_back(geometryContainer.getHash());

      m_instanceNumber++;

      return true;
    }

    bool RenderNode::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      bool isContained = false;

      std::list<uint64_t>& geometryList = m_meshes[geometryContainer.getMeshHandle()];
      for(std::list<uint64_t>::const_iterator geometryIterator = geometryList.begin(); geometryIterator != geometryList.end(); geometryIterator++)
      {
        if(*geometryIterator == geometryContainer.getHash())
        {
          geometryList.erase(geometryIterator);
          isContained = true;
          break;
        }
      }

      if(!isContained)//the node wasnt in the list --> wrong render node
      {
        return false;
      }

      m_instanceNumber--;

      m_instanceNumberChanged = true;

      if(m_meshes[geometryContainer.getMeshHandle()].size() == 0)
      {
        m_geometryChanged = true;

        m_meshes.erase(geometryContainer.getMeshHandle());
      }

      return true;
    }

    unsigned int RenderNode::getInstanceCount()
    {
      return m_instanceNumber;
    }

    bool RenderNode::hasGeometryChanged()
    {
      return m_geometryChanged;
    }

    bool RenderNode::hasInstanceNumberChanged()
    {
      return m_instanceNumberChanged;
    }

    bool RenderNode::isInstanced()
    {
      return false;
    }

    bool RenderNode::isEmpty()
    {
      return true;
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
      m_geometryChanged = false;
      m_instanceNumberChanged = false;
    }
	}
}
