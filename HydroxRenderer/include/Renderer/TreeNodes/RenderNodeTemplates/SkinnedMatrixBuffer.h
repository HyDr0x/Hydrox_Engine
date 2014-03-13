#ifndef SKINNEDMATRIXBUFFER_H_
#define SKINNEDMATRIXBUFFER_H_

#include <vector>

#include "Renderer/TreeNodes/RenderNodeTemplates/MatrixBufferInterface.h"

namespace he
{
	namespace renderer
	{
    class SkinnedMatrixBuffer : public MatrixBufferInterface
    {
    public:

      SkinnedMatrixBuffer(unsigned int maxBones);
      ~SkinnedMatrixBuffer();

      void resizeMatrixBuffer(unsigned int numInstances);
      void fillCaches(xBar::StaticGeometryContainer *geometryContainer, unsigned int instanceCounter);
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
