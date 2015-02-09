#ifndef SKINNEDRENDERNODE_H_
#define SKINNEDRENDERNODE_H_

#include <list>

#include <Utilities/Math/Math.hpp>

#include <XBar/SkinnedGeometryContainer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    class RenderOptions;

    class SkinnedRenderNode : public IRenderGroup
    {
    public:

      SkinnedRenderNode(util::SharedPointer<RenderOptions> options);
      virtual ~SkinnedRenderNode();

      virtual bool containsContainer(const xBar::IGeometryContainer& geometryContainer);

      virtual bool insertGeometry(const xBar::IGeometryContainer& geometryContainer) override;
      virtual bool removeGeometry(const xBar::IGeometryContainer& geometryContainer) override;

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

      virtual void rasterizeShadowGeometry() const override;

      virtual void rasterizeReflectiveShadowGeometry() const override;

      virtual void rasterizeIndexGeometry() const override;

      virtual void rasterizeGeometry() const override;

      virtual void updateBuffer() override;

      virtual unsigned int getCacheNumber() const override;

    protected:

      bool m_instanceNumberChanged;

      util::SharedPointer<RenderOptions> m_options;

      std::list<const xBar::SkinnedGeometryContainer> m_instances;
      std::list<const xBar::SkinnedGeometryContainer>::const_iterator m_iter;
    };
  }
}

#endif
