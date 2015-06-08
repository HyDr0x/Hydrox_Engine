#include "Renderer/Renderdata/SkinnedInstancesRenderData.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include <Shader/ComputeShader.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    SkinnedInstancesRenderData::SkinnedInstancesRenderData(util::SharedPointer<RenderOptions> options) :
      IInstanceRenderData(options),
      m_instanceNumberChanged(false)
    {
    }

    SkinnedInstancesRenderData::~SkinnedInstancesRenderData()
    {
    }

    bool SkinnedInstancesRenderData::containsContainer(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
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

    bool SkinnedInstancesRenderData::canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      return m_instances.size() < m_options->maxInstanceNumber;
    }

    void SkinnedInstancesRenderData::insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      m_instanceNumberChanged = true;

      m_instances.push_back(*geometryContainer.dynamic_pointer_cast<const xBar::SkinnedGeometryContainer>());
    }

    void SkinnedInstancesRenderData::removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      for(std::list<const xBar::SkinnedGeometryContainer>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
      {
        if((*instanceIterator) == *geometryContainer)
        {
          m_instances.erase(instanceIterator);
          m_instanceNumberChanged = true;

          return;
        }
      }    
    }

    unsigned int SkinnedInstancesRenderData::getInstanceNumber() const
    {
      return m_instances.size();
    }

    bool SkinnedInstancesRenderData::hasInstanceNumberChanged() const
    {
      return m_instanceNumberChanged;
    }

    void SkinnedInstancesRenderData::frustumCulling() const
    {
      unsigned int instanceNumber = m_instances.size();
      sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &instanceNumber);
      sh::ComputeShader::dispatchComputeShader(256, 1, 1);
    }

    void SkinnedInstancesRenderData::updateBuffer()
    {
      m_instanceNumberChanged = false;
    }
  }
}
