#ifndef RENDERCONTAINER_H_
#define RENDERCONTAINER_H_

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Renderdata/IRenderContainer.h"

namespace he
{
  namespace renderer
  {
    template<class MATERIALDATA, class DRAWDATA, class MATRIXDATA, class INSTANCEDATA> class RenderContainer : public IRenderContainer
    {
    public:

      RenderContainer(MATERIALDATA materialData, DRAWDATA drawData, MATRIXDATA matrixData, INSTANCEDATA instanceData) :
        m_materialData(materialData),
        m_drawData(drawData),
        m_matrixData(matrixData),
        m_instanceData(instanceData)
      {
      }

      virtual ~RenderContainer()
      {
      }

      virtual bool insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override
      {
        if(m_instanceData.canBeInserted(geometryContainer) && 
          m_materialData.canBeInserted(geometryContainer) &&
          m_drawData.canBeInserted(geometryContainer))
        {
          m_instanceData.insertGeometry(geometryContainer);
          m_materialData.insertGeometry(geometryContainer);
          m_drawData.insertGeometry(geometryContainer);

          return true;
        }

        return false;
      }

      virtual bool removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override
      {
        if(m_instanceData.containsContainer(geometryContainer))
        {
          m_instanceData.removeGeometry(geometryContainer);
          m_materialData.removeGeometry(geometryContainer);
          m_drawData.removeGeometry(geometryContainer);

          return true;
        }

        return false;
      }

      virtual bool isEmpty() const override
      {
        return m_instanceData.getInstanceNumber() == 0;
      }

      virtual void update() override
      {
        m_materialData.updateBuffer(m_instanceData);
        m_drawData.updateBuffer(m_instanceData);
        m_matrixData.updateBuffer(m_instanceData);
        m_instanceData.updateBuffer();
      }

      const IMaterialRenderData& getMaterialData() const override
      {
        return m_materialData;
      }

      const IDrawRenderData& getDrawData() const override
      {
        return m_drawData;
      }

      const IMatrixRenderData& getMatrixData() const override
      {
        return m_matrixData;
      }

      const IInstanceRenderData& getInstanceData() const override
      {
        return m_instanceData;
      }

      VertexElementFlags getMeshVertexDeclaration() const override
      {
        return m_drawData.getMeshVertexDeclaration();
      }

      const std::vector<std::vector<util::ResourceHandle>>& getTextureHandles() const override
      {
        return m_materialData.getTextureHandles();
      }

      virtual unsigned int getPerInstanceCacheNumber() const override
      {
        return m_drawData.getPerInstanceCacheNumber();
      }

      virtual unsigned int getPerInstanceOccluderNumber() const override
      {
        return m_drawData.getPerInstanceOccluderNumber();
      }

      virtual unsigned int getPerInstanceVertexNumber() const override
      {
        return m_drawData.getPerInstanceVertexNumber();
      }

      virtual unsigned int getPerInstanceTriangleNumber() const override
      {
        return m_drawData.getPerInstanceTriangleNumber();
      }

    private:

      MATERIALDATA m_materialData;
      DRAWDATA m_drawData;
      MATRIXDATA m_matrixData;
      INSTANCEDATA m_instanceData;
    };
  }
}

#endif
