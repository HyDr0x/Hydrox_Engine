#ifndef MATRIXBUFFERINTERFACE_H_
#define MATRIXBUFFERINTERFACE_H_

#include <vector>

#include "Renderer/Buffer/GPUBuffer.h"

namespace he
{
  namespace xBar
  {
    class StaticGeometryContainer;
  }

	namespace renderer
	{
    class StaticGeometryContainer;

    class MatrixBufferInterface
    {
    public:

      virtual void resizeMatrixBuffer(unsigned int numInstances) = 0;

      virtual void fillCaches(xBar::StaticGeometryContainer *geometryContainer, unsigned int instanceCounter) = 0;
      virtual void fillBuffer(unsigned int numInstances) = 0;

      virtual void bindTransformationMatrixBuffer() = 0;
      virtual void bindBBoxMatrixBuffer() = 0;

      virtual void unbindTransformationMatrixBuffer() = 0;
      virtual void unbindBBoxMatrixBuffer() = 0;

    protected:

      GPUBuffer m_matrixBuffer;
      std::vector<GLubyte> m_matrixCache;
    };
  }
}

#endif