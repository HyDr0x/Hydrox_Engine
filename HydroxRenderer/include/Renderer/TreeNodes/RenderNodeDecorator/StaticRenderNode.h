#ifndef STATICRENDERNODE_H_
#define STATICRENDERNODE_H_

#include <list>

#include <Utilities/Math/Math.hpp>

#include <XBar/StaticGeometryContainer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

namespace he
{
  namespace renderer
  {
    struct RenderOptions;

    class StaticRenderNode : public IRenderNode
    {
    public:

      StaticRenderNode(const RenderOptions& options);
      virtual ~StaticRenderNode();

      virtual bool preTraverse(Traverser* traverser) override;
      virtual void postTraverse(Traverser* traverser) override;

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

      virtual void rasterizeGeometry() const override;

      virtual void updateBuffer() override;

      virtual unsigned int getMaxGeometry() const override;
      virtual unsigned int getMaxMaterials() const override;
      virtual unsigned int getMaxBones() const override;

    protected:

      bool m_instanceNumberChanged;

      const RenderOptions& m_options;

      std::list<const xBar::StaticGeometryContainer> m_instances;
      std::list<const xBar::StaticGeometryContainer>::const_iterator m_iter;
    };
  }
}

#endif