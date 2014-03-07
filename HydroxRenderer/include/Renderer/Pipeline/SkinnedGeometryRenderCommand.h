#ifndef SKINNEDGEOMETRYRENDERCOMMAND_H_
#define SKINNEDGEOMETRYRENDERCOMMAND_H_

#include <vector>

#include "Renderer/Pipeline/GeometryCommandInterface.h"

namespace he
{
	namespace renderer
	{
    class SkinnedGeometryRenderCommand : public GeometryCommandInterface
    {
    public:

      SkinnedGeometryRenderCommand(unsigned int maxBones);
      ~SkinnedGeometryRenderCommand();

      void resizeMatrixBuffer(unsigned int numInstances);
      void fillCaches(sg::GeoNode *geoNode, unsigned int instanceCounter);
      void fillBuffer(unsigned int numInstances);
      void bindTransformationMatrixBuffer();
      void bindBBoxMatrixBuffer();
      void unbindTransformationMatrixBuffer();
      void unbindBBoxMatrixBuffer();

    private:

      GPUBuffer m_bboxMatrixBuffer;
      std::vector<GLubyte> m_bboxMatrixCache;

      unsigned int m_maxBones;
    };
	}
}

#endif
