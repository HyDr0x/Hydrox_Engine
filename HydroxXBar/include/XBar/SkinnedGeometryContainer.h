#ifndef SKINNEDGEOMETRYCONTAINER_H_
#define SKINNEDGEOMETRYCONTAINER_H_

#include <vector>

#include "XBar/StaticGeometryContainer.h"

#include "XBar/DLLExport.h"

namespace he
{
	namespace xBar
	{
    class GRAPHICAPI SkinnedGeometryContainer : public StaticGeometryContainer
    {
    public:

      SkinnedGeometryContainer(std::vector<util::Matrix<float, 4>> *boneTransformMatrices, 
                               std::vector<util::Matrix<float, 4>> *inverseBindPoseMatrices, 
                               util::Matrix<float, 4> *trfMatrix, 
                               util::ResourceHandle materialHandle, 
                               util::ResourceHandle meshHandle);
      ~SkinnedGeometryContainer();

      SkinnedGeometryContainer* clone();

      std::vector<util::Matrix<float, 4>> getSkinningMatrices();

    private:

      void createHash();

      std::vector<util::Matrix<float, 4>> *m_boneTransformMatrices;
      std::vector<util::Matrix<float, 4>> *m_inverseBindPoseMatrices;
    };
  }
}

#endif