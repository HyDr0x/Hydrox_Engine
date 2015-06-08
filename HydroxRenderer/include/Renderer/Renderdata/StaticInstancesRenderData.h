#ifndef STATICINSTANCESRENDERDATA_H_
#define STATICINSTANCESRENDERDATA_H_

#include <list>

#include <XBar/StaticGeometryContainer.h>

#include "Renderer/Renderdata/IInstanceRenderData.h"

namespace he
{
  namespace renderer
  {
    class RenderOptions;

    class StaticInstancesRenderData : public IInstanceRenderData
    {
    public:

      StaticInstancesRenderData(util::SharedPointer<RenderOptions> options);
      virtual ~StaticInstancesRenderData();

      virtual bool containsContainer(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;

      virtual bool canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;

      virtual void insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;
      virtual void removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;

      inline virtual bool isEndInstanceIterator() const
      {
        return m_iter == m_instances.end();
      }

      inline virtual void resetInstanceIterator()
      {
        m_iter = m_instances.begin();
      }

      inline virtual const xBar::IGeometryContainer& incInstanceIterator()
      {
        const xBar::IGeometryContainer& instance = *m_iter;
        m_iter++;
        return instance;
      }

      virtual unsigned int getInstanceNumber() const override;

      virtual bool hasInstanceNumberChanged() const override;

      virtual void frustumCulling() const override;

      virtual void updateBuffer() override;

    protected:

      bool m_instanceNumberChanged;

      std::list<const xBar::StaticGeometryContainer> m_instances;
      std::list<const xBar::StaticGeometryContainer>::const_iterator m_iter;
    };
  }
}

#endif
