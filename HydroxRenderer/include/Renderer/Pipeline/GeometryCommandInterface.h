#ifndef GEOMETRYCOMMANDINTERFACE_H_
#define GEOMETRYCOMMANDINTERFACE_H_

#include <vector>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include "Renderer/Buffer/GPUBuffer.h"

namespace he
{
	namespace renderer
	{
    class GeometryCommandInterface
    {
    public:

      virtual void resizeMatrixBuffer(unsigned int numInstances) = 0;
      virtual void fillCaches(sg::GeoNode *geoNode, unsigned int instanceCounter) = 0;
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