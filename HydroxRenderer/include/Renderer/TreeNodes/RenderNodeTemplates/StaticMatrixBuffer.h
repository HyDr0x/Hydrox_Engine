#ifndef STATICMATRIXBUFFER_H_
#define STATICMATRIXBUFFER_H_

#include "Renderer/TreeNodes/RenderNodeTemplates/MatrixBufferInterface.h"

namespace he
{
	namespace renderer
	{
    class StaticMatrixBuffer : public MatrixBufferInterface
    {
    public:

      StaticMatrixBuffer();
      ~StaticMatrixBuffer();

      void resizeMatrixBuffer(unsigned int numInstances);
      void fillCaches(xBar::StaticGeometryContainer *geometryContainer, unsigned int instanceCounter);
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
