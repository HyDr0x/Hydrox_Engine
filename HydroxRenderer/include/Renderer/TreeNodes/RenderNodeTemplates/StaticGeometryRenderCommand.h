#ifndef STATICGEOMETRYRENDERCOMMAND_H_
#define STATICGEOMETRYRENDERCOMMAND_H_

#include "Renderer/TreeNodes/RenderNodeTemplates/GeometryCommandInterface.h"

namespace he
{
	namespace renderer
	{
    class StaticGeometryRenderCommand : public GeometryCommandInterface
    {
    public:

      StaticGeometryRenderCommand();
      ~StaticGeometryRenderCommand();

      void resizeMatrixBuffer(unsigned int numInstances);
      void fillCaches(sg::GeoNode *geoNode, unsigned int instanceCounter);
      void fillBuffer(unsigned int numInstances);
      void bindTransformationMatrixBuffer();
      void bindBBoxMatrixBuffer();
      void unbindTransformationMatrixBuffer();
      void unbindBBoxMatrixBuffer();

    private:

    };
	}
}

#endif
