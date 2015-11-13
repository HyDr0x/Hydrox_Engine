#ifndef IRENDERCONTAINER_H_
#define IRENDERCONTAINER_H_

#include "Renderer/Renderdata/IMaterialRenderData.h"
#include "Renderer/Renderdata/IDrawRenderData.h"
#include "Renderer/Renderdata/IMatrixRenderData.h"
#include "Renderer/Renderdata/IInstanceRenderData.h"

#include <DataBase/Mesh.h>

namespace he
{
  namespace renderer
  {
    class IRenderContainer
    {
    public:

      virtual ~IRenderContainer()
      {}

      virtual bool insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;
      virtual bool removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual bool isEmpty() const = 0;

      virtual void update() = 0;

      virtual const IMaterialRenderData& getMaterialData() const = 0;

      virtual const IDrawRenderData& getDrawData() const = 0;

      virtual const IMatrixRenderData& getMatrixData() const = 0;

      virtual const IInstanceRenderData& getInstanceData() const = 0;

      virtual VertexElementFlags getMeshVertexDeclaration() const = 0;

      virtual const std::vector<std::vector<util::ResourceHandle>>& getTextureHandles() const = 0;

      virtual unsigned int getPerInstanceCacheNumber() const = 0;

      virtual unsigned int getPerInstanceOccluderNumber() const = 0;

      virtual unsigned int getPerInstanceVertexNumber() const = 0;

      virtual unsigned int getPerInstanceTriangleNumber() const = 0;
    };
  }
}

#endif
