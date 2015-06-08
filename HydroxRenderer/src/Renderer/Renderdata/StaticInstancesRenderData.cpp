#include "Renderer/Renderdata/StaticInstancesRenderData.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include <Shader/ComputeShader.h>

namespace he
{
  namespace renderer
  {
    StaticInstancesRenderData::StaticInstancesRenderData(util::SharedPointer<RenderOptions> options) :
      IInstanceRenderData(options),
      m_instanceNumberChanged(false)
    {
    }

    StaticInstancesRenderData::~StaticInstancesRenderData()
    {
    }

    bool StaticInstancesRenderData::containsContainer(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      for(std::list<const xBar::StaticGeometryContainer>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
      {
        if((*instanceIterator) == *geometryContainer)
        {
          return true;
        }
      }

      return false;
    }

    bool StaticInstancesRenderData::canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      return m_instances.size() < m_options->maxInstanceNumber;
    }

    void StaticInstancesRenderData::insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      m_instanceNumberChanged = true;

      m_instances.push_back(*geometryContainer.dynamic_pointer_cast<const xBar::StaticGeometryContainer>());
    }

    void StaticInstancesRenderData::removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      for(std::list<const xBar::StaticGeometryContainer>::iterator instanceIterator = m_instances.begin(); instanceIterator != m_instances.end(); instanceIterator++)
      {
        if((*instanceIterator) == *geometryContainer)
        {
          m_instances.erase(instanceIterator);
          m_instanceNumberChanged = true;

          return;
        }
      }    
    }

    unsigned int StaticInstancesRenderData::getInstanceNumber() const
    {
      return m_instances.size();
    }

    bool StaticInstancesRenderData::hasInstanceNumberChanged() const
    {
      return m_instanceNumberChanged;
    }

    void StaticInstancesRenderData::frustumCulling() const
    {
      unsigned int instanceNumber = m_instances.size();
      sh::ComputeShader::setUniform(0, GL_UNSIGNED_INT, &instanceNumber);
      sh::ComputeShader::dispatchComputeShader(256, 1, 1);
    }

    void StaticInstancesRenderData::updateBuffer()
    {
      m_instanceNumberChanged = false;
    }
  }
}
